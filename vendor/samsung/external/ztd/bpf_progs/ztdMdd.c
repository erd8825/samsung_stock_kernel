#include "bpf_shared.h"
#include "bpf_read.h"
#include <stdbool.h>
#include <ztd_events.h>
#include <ztd_sc_shared.h>
#include <ztd_mdd_shared.h>

#define DEBUG 0
#define DEBUG_ENTRY 0

// Flags
#define O_CREAT 0x40

#define PROT_READ       0x01
#define PROT_WRITE      0x02
#define PROT_EXEC       0X03
#define MAP_ANONYMOUS   0x20
#define MAP_SHARED      0x01
#define MAP_PRIVATE     0x02
#define MAP_FIXED       0x010

#define LOAD_TYPE_UNKNOWN   0
#define LOAD_TYPE_DLOPEN    1
#define LOAD_TYPE_APK       2
#define LOAD_TYPE_DEX       3

// /data/user/... on Little Endian notation
#define DATA_USER_HIGH 0x73752F617461642F
#define DATA_USER_LOW  0x2F7265

// /storage/... on Little Endian notation
#define STORAGE_HIGH 0x656761726F74732F
#define STORAGE_LOW  0x2F

// /sdcard/... on Little Endian notation
#define SDCARD_HIGH 0x2F6472616364732F

// File signatures
#define DEX_FILE            0x6465780A
#define ZIP_JAR_APK_FILE    0x504B0304
#define RAR_FILE            0x52617221
#define SO_FILE             0x7F454C46
#define VDEX_FILE           0x76646578

DEFINE_BPF_MAP_GRO(mdd_open_enter_data_map, LRU_HASH, uint64_t, mdd_event_raw_t, 128, AID_ROOT);
DEFINE_BPF_MAP_GRO(mdd_open_exit_data_map, LRU_HASH, mdd_key_aux_map_t, mdd_event_raw_t, 64, AID_ROOT);
DEFINE_BPF_MAP_GRO(mdd_mmap_enter_data_map, LRU_HASH, uint64_t, mdd_event_raw_t, 32, AID_ROOT);
DEFINE_BPF_MAP_GRO(mdd_mmap_exit_data_map, LRU_HASH, uint64_t, mdd_event_raw_t, 32, AID_ROOT);
DEFINE_BPF_MAP_HASH_NPA(mdd_data_map, uint64_t, mdd_event_raw_t, 128, AID_SYSTEM);
DEFINE_BPF_MAP_GRO(mdd_event_map, PERCPU_ARRAY, uint32_t, mdd_event_raw_t, 1, AID_SYSTEM);

DEFINE_BPF_MAP_HASH_NPA(mdd_uids_filter_map, uint32_t, mdd_uids_filter_data_t, 100, AID_SYSTEM);

DEFINE_BPF_RINGBUF(event_noti_ringbuf, event_noti_t, 4096, AID_ROOT, AID_SYSTEM, 0660);

static inline __always_inline bool updateRawData(sys_enter_data_t *args, mdd_event_raw_t *data) {
    bpf_probe_read_user_str(data->filepath, sizeof(data->filepath), POINTER_OF_USER_SPACE(args->args[1]));
    data->flags = (int) args->args[2];

    uint64_t key = bpf_get_current_pid_tgid();
    bpf_mdd_open_enter_data_map_update_elem(&key, data, BPF_ANY);

    return true;
}

#if DEBUG
static inline __always_inline void dumpRawData(const char *tag, mdd_event_raw_t *data) {
    bpf_printk("[ztd] %s :: event = %d", tag, data->event);
    bpf_printk("[ztd] %s :: event_time = %lu", tag, data->event_time);
    bpf_printk("[ztd] %s :: pid = %u", tag, data->pid);
    bpf_printk("[ztd] %s :: uid = %u", tag, data->uid);
    bpf_printk("[ztd] %s :: filepath = %s", tag, data->filepath);
    bpf_printk("[ztd] %s :: signature = 0x%x", tag, data->signature);
    bpf_printk("[ztd] %s :: flags = %d", tag, data->flags);
}
#endif

static inline __always_inline void sendRawData(
#if DEBUG
    const char *tag, 
#endif
    mdd_event_raw_t *data) {
    event_noti_t *noti = bpf_event_noti_ringbuf_reserve();
    if (!noti) {
#if DEBUG
        bpf_printk("[ztd] %s :: no ringbuf to reserve!", tag);
#endif
        return;
    }
    uint64_t cpu_id = bpf_get_smp_processor_id(); 
    uint64_t key = (cpu_id & 0x00000000000000FF) << 56 | data->event_time;
#if DEBUG
    bpf_printk("[ztd] %s :: type: %d, key: %lu", tag, data->event, key);
#endif
    bpf_mdd_data_map_update_elem(&key, data, BPF_ANY);

    noti->type = data->event;
    noti->key = key;

    bpf_event_noti_ringbuf_submit(noti);
}

static inline __always_inline bool hasCreateFileFlag(int flags) {
    return (flags & O_CREAT) != 0;
}

int isTargetPath(const char* filePath) {
    if (filePath == NULL) {
        return 0;
    }
    mdd_path_filter_bytes_t *p = (mdd_path_filter_bytes_t*)filePath;

    if (p->high == DATA_USER_HIGH) {
        return (p->low & 0xffffff) == DATA_USER_LOW;
    }
    
    if (p->high == STORAGE_HIGH) {
        return (p->low & 0xff) == STORAGE_LOW;
    }
    
    return (p->high == SDCARD_HIGH);
}

static inline __always_inline uint32_t getLoadTypeFromSignature(int signature) {
    switch (signature) {
        case SO_FILE:
            return LOAD_TYPE_DLOPEN;
        case ZIP_JAR_APK_FILE:
            return LOAD_TYPE_APK;
        case DEX_FILE:
        case VDEX_FILE:
            return LOAD_TYPE_DEX;
        default:
            return LOAD_TYPE_UNKNOWN;
    }
}

static inline __always_inline void onSyscallOpenEnter(sys_enter_data_t *args) {
    uid_t uid = (bpf_get_current_uid_gid() & 0xFFFFFFFF);
    if (bpf_mdd_uids_filter_map_lookup_elem(&uid) == NULL) {
        return;
    }
    
    // Flag filter
    int flag = (int) args->args[2];
    if (!hasCreateFileFlag(flag)) {
        return;
    }
    
    // Path prefix filter
    mdd_path_filter_data_t pathPrefix = {};
    bpf_probe_read_user(pathPrefix.target, sizeof(pathPrefix.target), POINTER_OF_USER_SPACE(args->args[1]));
    if (!isTargetPath(pathPrefix.target)) {
        return;
    }

    uint32_t zero = 0; // Look-up key
    mdd_event_raw_t *data = bpf_mdd_event_map_lookup_elem(&zero);
    if (!data) {
        return;
    }

    bool res = updateRawData(args, data);
    if (!res) {
        return;
    }
}

static inline __always_inline void onSyscallOpenExit(sys_exit_data_t *args) {
    uint64_t key = bpf_get_current_pid_tgid();
    mdd_event_raw_t  *data = bpf_mdd_open_enter_data_map_lookup_elem(&key);

    if (data && args->ret > 0) {
        mdd_key_aux_map_t mdd_key = {
            .fd = args->ret,
            .pid = bpf_get_current_pid_tgid(),
            .uid = (bpf_get_current_uid_gid() & 0xFFFFFFFF)
        };
    
        bpf_mdd_open_exit_data_map_update_elem(&mdd_key, data, BPF_ANY);
 
#if DEBUG
        bpf_printk("[ztd] ztdMddOpen.sys_exit :: filepath = %s", data->filepath);
        bpf_printk("[ztd] ztdMddOpen.sys_exit :: fd = %d", mdd_key.fd);
#endif
    }

    bpf_mdd_open_enter_data_map_delete_elem(&key);
}

static inline __always_inline void onSyscallWriteEnter(sys_enter_data_t *args) {
    mdd_key_aux_map_t key = {
        .fd = (int) args->args[0],
        .pid = bpf_get_current_pid_tgid(),
        .uid = (bpf_get_current_uid_gid() & 0xFFFFFFFF)
    };
 
    mdd_event_raw_t *data = bpf_mdd_open_exit_data_map_lookup_elem(&key);
    if (data == NULL) {
        return;
    }

    // Four leading bytes
    char file_signature[4];
    bpf_probe_read_user(file_signature, sizeof(file_signature), POINTER_OF_USER_SPACE(args->args[1]));
    int signature = (file_signature[0] << 24 | file_signature[1] << 16 | file_signature[2] << 8 | file_signature[3]);

#if DEBUG
    bpf_printk("[ztd] ztdMddWrite.sys_enter :: fd = %d", key.fd);
    bpf_printk("[ztd] ztdMddWrite.sys_enter :: signature = 0x%x", signature);
#endif

    if (signature == ZIP_JAR_APK_FILE || signature == RAR_FILE || signature == SO_FILE || signature == DEX_FILE || signature == VDEX_FILE) {
        data->event = TRACE_EVENT_MASQUERADE_FILE;
        data->event_time = bpf_ktime_get_boot_ns();
        data->pid = key.pid;
        data->uid = key.uid;
        data->signature = signature;
 
#if DEBUG
        const char tag[] = "ztdMddWrite.sys_enter";
        dumpRawData(tag, data);
        sendRawData(tag, data);
#else
        sendRawData(data);
#endif
    }

    uint32_t zero = 0; // Look-up key
    mdd_event_raw_t *load_data = bpf_mdd_event_map_lookup_elem(&zero);
    if (!load_data) {
        return;
    }
    load_data->pid = key.pid;
    load_data->uid = key.uid;
    load_data->load_type = getLoadTypeFromSignature(signature);
    bpf_probe_read_kernel(load_data->filepath, sizeof(load_data->filepath), data->filepath);
    uint64_t load_key = bpf_get_current_pid_tgid();
    bpf_mdd_mmap_enter_data_map_update_elem(&load_key, load_data, BPF_ANY);

    bpf_mdd_open_exit_data_map_delete_elem(&key);
}

static inline __always_inline void onSyscallMmapEnter(sys_enter_data_t *args) {
    uint64_t key = bpf_get_current_pid_tgid();
    
    mdd_event_raw_t *load_data = bpf_mdd_mmap_enter_data_map_lookup_elem(&key);
    if (!load_data) {
        return;
    }
    uint32_t prot = args->args[2];
    uint32_t flags = args->args[3];
    if (flags & MAP_ANONYMOUS) {
#if DEBUG
        bpf_printk("[ztd] ztdMddMmap.sys_enter :: Anonymous mmap detected! Exiting mmap() process...");
#endif
        return;
    }
#if DEBUG
    bpf_printk("[ztd] ztdMddMmap.sys_enter :: Entry found for fd = %d", args->args[4]);
#endif
    
    // Validation of specific file patter
    bool valid_pattern = false;
    switch (load_data->load_type) {
        case LOAD_TYPE_DLOPEN:
            valid_pattern = !(flags & MAP_ANONYMOUS) && !(prot & PROT_EXEC);
            break;
        case LOAD_TYPE_APK:
            valid_pattern = !(flags & MAP_ANONYMOUS) && (flags & (MAP_SHARED | MAP_PRIVATE));
            break;
        case LOAD_TYPE_DEX:
            valid_pattern = !(flags & MAP_ANONYMOUS) && (prot & PROT_READ);
            break;
        default: 
            break;
    }
    if (!valid_pattern) {
#if DEBUG
        bpf_printk("[ztd] ztdMddMmap.sys_enter :: Invalid mmap pattern for %d load type", load_data->load_type);
        bpf_printk("[ztd] ztdMddMmap.sys_enter :: FILE: %s | PROT: 0x%x | MAP: 0x%x", load_data->filepath, prot, flags);
#endif
        bpf_mdd_mmap_enter_data_map_delete_elem(&key);
        return;
    }
#if DEBUG
    bpf_printk("[ztd] ztdMddMmap.sys_enter :: File-based mmap detected: %s (type: %d)", load_data->filepath, load_data->load_type);
#endif
    load_data->prot = prot;
    load_data->flags = flags;
    load_data->map_size = (uint64_t) args->args[1];
    bpf_mdd_mmap_exit_data_map_update_elem(&key, load_data, BPF_ANY);
    
    bpf_mdd_mmap_enter_data_map_delete_elem(&key);
}

static inline __always_inline void onSyscallMmapExit(sys_exit_data_t *args) {
    uint64_t key = bpf_get_current_pid_tgid();
    mdd_event_raw_t *load_data = bpf_mdd_mmap_exit_data_map_lookup_elem(&key);
    if (!load_data) {
        return;
    }

    if (load_data && args->ret > 0) {
        load_data->event = TRACE_EVENT_LOAD_DROPPED_FILE;
        load_data->event_time = bpf_ktime_get_boot_ns();
        load_data->mapped_addr = (uint64_t) args->ret;

#ifdef DEBUG
        bpf_printk("[ztd] ztdMddMmap.sys_exit :: file = %s | addr = 0x%llx | size = %lu", 
            load_data->filepath, load_data->mapped_addr, load_data->map_size);
#endif
#if DEBUG
        const char tag[] = "ztdMddMmap.sys_exit";
        dumpRawData(tag, load_data);
        sendRawData(tag, load_data);
#else
        sendRawData(load_data);
#endif
    }

    bpf_mdd_mmap_exit_data_map_delete_elem(&key);
}

DEFINE_SC_TRACEPOINT(raw_syscalls, sys_enter, mdd_enter)
(sys_enter_data_t *args) {
    if (args->id == GENERIC_SYSCALL_NR_OPEN) {
#if DEBUG_ENTRY
        bpf_printk("[ztd] sys_enter :: NR = %ld (OPEN)", args->id);
#endif
        onSyscallOpenEnter(args);
    } else if (args->id == GENERIC_SYSCALL_NR_WRITE) {
#if DEBUG_ENTRY
        bpf_printk("[ztd] sys_enter :: NR = %ld (WRITE)", args->id);
#endif
        onSyscallWriteEnter(args);
    } else if (args->id == GENERIC_SYSCALL_NR_MMAP) {
#if DEBUG_ENTRY
        bpf_printk("[ztd] sys_enter :: NR = %ld (MMAP)", args->id);
#endif
        onSyscallMmapEnter(args);
    }
    return 1;
}

DEFINE_SC_TRACEPOINT(raw_syscalls, sys_exit, mdd_exit)
(sys_exit_data_t *args) {
    if (args->id == GENERIC_SYSCALL_NR_OPEN) {
#if DEBUG_ENTRY
        bpf_printk("[ztd] sys_exit :: NR = %ld (OPEN)", args->id);
#endif
        onSyscallOpenExit(args);
    } else if (args->id == GENERIC_SYSCALL_NR_MMAP) {
#if DEBUG_ENTRY
        bpf_printk("[ztd] sys_exit :: NR = %ld (MMAP)", args->id);
#endif
        onSyscallMmapExit(args);
    }
    return 1;
}

LICENSE("GPL");