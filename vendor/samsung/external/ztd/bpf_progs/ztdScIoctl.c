#include "bpf_shared.h"
#include "bpf_read.h"
#include <stdbool.h>
#include <ztd_events.h>
#include <ztd_sc_shared.h>
#include <ztd_ioctl_shared.h>

#define DEBUG 0
#define DEBUG_ENTRY 0

// Little Endian Notation
// /dev/
#define DEV_PATH            0x2F7665642F
// /dev/ashmem...
#define ASHMEM_PATH_HIGH    0x6873612F7665642F
#define ASHMEM_PATH_LOW     0x6D656D
// kgsl
#define KGSL_PATH           0x6C73676B

#define CURRENT_UID (bpf_get_current_uid_gid() & 0xFFFFFFFF)
#define FIRST_APPLICATION_UID       10000
#define LAST_APPLICATION_UID        19999
#define USER_RANGE                  100000
#define CHECK_TRACEABLE_UID if (((CURRENT_UID % USER_RANGE) < FIRST_APPLICATION_UID) || ((CURRENT_UID % USER_RANGE) > LAST_APPLICATION_UID)) return 0;

DEFINE_BPF_MAP_GRW(sc_ioctl_data_map, HASH, ioctl_key_map_t, ioctl_event_raw_t, 2048, AID_SYSTEM);

DEFINE_BPF_MAP_GRO(ioctl_aux_map, HASH, ioctl_key_aux_map_t, ioctl_sc_open_data_t, 100, AID_ROOT);
DEFINE_BPF_MAP_GRO(sc_open_data_map, HASH, uint64_t, ioctl_sc_open_data_t, 100, AID_ROOT);

DEFINE_BPF_MAP_GRW(ioctl_uids_filter_map, HASH, uint32_t, uids_filter_data_t, 1024, AID_SYSTEM);
DEFINE_BPF_MAP_GRW(ioctl_paths_filter_map, HASH, paths_filter_data_t, paths_filter_data_t, 128, AID_SYSTEM);

#if DEBUG
static inline __always_inline void dumpRawOpenData(const char *tag, ioctl_sc_open_data_t *data) {
    bpf_printk("[ztd] %s :: fd = %d", tag, data->fd);
    bpf_printk("[ztd] %s :: filename = %s", tag, data->filename);
}
#endif

#if DEBUG
static inline __always_inline void dumpRawIoctlData(const char *tag, ioctl_event_raw_t *data) {
    bpf_printk("[ztd] %s :: uid = %u", tag, data->uid);
    bpf_printk("[ztd] %s :: event_time = %d", tag, data->event_time);
    bpf_printk("[ztd] %s :: op_code = 0x%x", tag, data->op_code);
    bpf_printk("[ztd] %s :: counter = %d", tag, data->counter);
    bpf_printk("[ztd] %s :: driver_path = %s", tag, data->driver_path);
}
#endif

static inline __always_inline void updateRawOpenData(sys_enter_data_t *args) {
    ioctl_sc_open_data_t data = {};
    bpf_probe_read_user_str(data.filename, sizeof(data.filename), POINTER_OF_USER_SPACE(args->args[1]));

    uint64_t key = bpf_get_current_pid_tgid();
    bpf_sc_open_data_map_update_elem(&key, &data, BPF_ANY);
}

static inline __always_inline bool isDevPath(ioctl_path_bytes_t* buffer) {
    if (buffer == NULL) {
        return false;
    }

    return (buffer->data[0] & 0x000000ffffffffff) == DEV_PATH;
}

static inline __always_inline bool isKgslFile(ioctl_path_bytes_t* buffer) {
    if (buffer == NULL) {
        return false;
    }

    const uint8_t *buf = (const uint8_t*) buffer->data;

    for (int i = 0; i <= 60 && buf[i] != '\0' && buf[i + 1] != '\0' && buf[ i + 2] != '\0' && buf[i + 3] != '\0'; i++) {
        if (buf[i] == 'k' && buf[i + 1] == 'g' && buf[i + 2] == 's' && buf[i + 3] == 'l') {
            return true;
        }
    }

    return false;
}

static inline __always_inline bool isAshmemFile(ioctl_path_bytes_t* buffer) {
    if (buffer == NULL) {
        return false;
    }

    return (buffer->data[0] == ASHMEM_PATH_HIGH) && ((buffer->data[1] & 0x0000000000ffffff) == ASHMEM_PATH_LOW);
}

static inline __always_inline void onSyscallOpenEnter(sys_enter_data_t *args) {
    uid_t uid = CURRENT_UID;
    if (bpf_ioctl_uids_filter_map_lookup_elem(&uid) != NULL) {
        return;
    }

    paths_filter_data_t file = {};
    bpf_probe_read_user_str(file.path, sizeof(file.path), POINTER_OF_USER_SPACE(args->args[1]));

    if (bpf_ioctl_paths_filter_map_lookup_elem(&file) == NULL) {
        ioctl_path_bytes_t* buffer = (ioctl_path_bytes_t*)file.path;
        if (isDevPath(buffer)) {
            if (isKgslFile(buffer)) {
                bpf_ioctl_paths_filter_map_update_elem(&file, &file, BPF_ANY);
                updateRawOpenData(args);
                return;
            }

            if (isAshmemFile(buffer)) {
                bpf_ioctl_paths_filter_map_update_elem(&file, &file, BPF_ANY);
                updateRawOpenData(args);
                return;
            }
        }

        return;
    }

    updateRawOpenData(args);
}

static inline __always_inline void onSyscallOpenExit(sys_exit_data_t *args) {
    uint64_t key_open_data = bpf_get_current_pid_tgid();
    ioctl_sc_open_data_t *open_data = bpf_sc_open_data_map_lookup_elem(&key_open_data);

    if (open_data && args->ret > 0) {
        ioctl_key_aux_map_t key = {
            .fd = args->ret,
            .uid = CURRENT_UID
        };

        open_data->fd = args->ret;
        bpf_ioctl_aux_map_update_elem(&key, open_data, BPF_ANY);
        bpf_sc_open_data_map_delete_elem(&key_open_data);
#if DEBUG
        const char tag[] = "ztdScIoctl.sys_open_exit";
        dumpRawOpenData(tag, open_data);
        bpf_printk("[ztd] %s :: sc open data updated", tag);
#endif
    }
}

static inline __always_inline void updateRawIoctlData(sys_enter_data_t *args, ioctl_key_aux_map_t *key, ioctl_sc_open_data_t *open_data) {
    uint32_t op_code = (uint32_t)args->args[1];
    
    ioctl_key_map_t ioctl_key = {
        .uid = key->uid,
        .fd = key->fd,
        .op_code = op_code
    };
    __builtin_memcpy(&ioctl_key.driver_path, open_data->filename, sizeof(ioctl_key.driver_path));

    ioctl_event_raw_t *ioctl_data = bpf_sc_ioctl_data_map_lookup_elem(&ioctl_key);
    if (ioctl_data == NULL) {
        ioctl_event_raw_t data = {};

        data.uid = key->uid;
        data.event_time = bpf_ktime_get_boot_ns() / 1000000ULL;
        __builtin_memcpy(&data.driver_path, open_data->filename, sizeof(data.driver_path));
        data.op_code = op_code;
        __sync_fetch_and_add(&data.counter, 1);
        bpf_sc_ioctl_data_map_update_elem(&ioctl_key, &data, BPF_ANY);
#if DEBUG
        const char tag[] = "ztdScIoctl.sys_ioctl_enter";
        dumpRawIoctlData(tag, &data);
        bpf_printk("[ztd] %s :: sc ioctl data updated", tag);
#endif
    } else {
        __sync_fetch_and_add(&ioctl_data->counter, 1);
        bpf_sc_ioctl_data_map_update_elem(&ioctl_key, ioctl_data, BPF_ANY);
#if DEBUG
        const char tag[] = "ztdScIoctl.sys_ioctl_enter";
        dumpRawIoctlData(tag, ioctl_data);
        bpf_printk("[ztd] %s :: sc ioctl data updated", tag);
#endif
    }
}

static inline __always_inline void onSyscallIoctlEnter(sys_enter_data_t *args) {
    ioctl_key_aux_map_t key = {
        .fd = (int) args->args[0],
        .uid = CURRENT_UID
    };
    
    ioctl_sc_open_data_t *open_data = bpf_ioctl_aux_map_lookup_elem(&key);
    if (open_data == NULL) {
        return;
    }

    updateRawIoctlData(args, &key, open_data);
}


static inline __always_inline void onSyscallCloseEnter(sys_enter_data_t *args) {
    ioctl_key_aux_map_t key = {
        .fd = (int) args->args[0],
        .uid = CURRENT_UID
    };

    if (bpf_ioctl_aux_map_lookup_elem(&key) == NULL) {
        return;
    }

    bpf_ioctl_aux_map_delete_elem(&key);
    
#if DEBUG
    bpf_printk("[ztd] :: fd: %d closed", key.fd);
#endif
}

DEFINE_SC_TRACEPOINT(raw_syscalls, sys_enter, sc_ioctl_enter)
(sys_enter_data_t *args) {
    CHECK_TRACEABLE_UID;

    if (args->id == GENERIC_SYSCALL_NR_OPEN) {
        onSyscallOpenEnter(args);
    } else if (args->id == GENERIC_SYSCALL_NR_IOCTL) {
        onSyscallIoctlEnter(args);
    } else if (args->id == GENERIC_SYSCALL_NR_CLOSE) {
        onSyscallCloseEnter(args);
    }

    return 1;
}

DEFINE_SC_TRACEPOINT(raw_syscalls, sys_exit, sc_ioctl_exit)
(sys_exit_data_t *args) {
    CHECK_TRACEABLE_UID;

    if (args->id == GENERIC_SYSCALL_NR_OPEN) {
        onSyscallOpenExit(args);
    }

    return 1;
}

LICENSE("GPL");