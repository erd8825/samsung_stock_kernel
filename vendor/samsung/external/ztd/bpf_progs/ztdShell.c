#include "bpf_shared.h"
#include "bpf_read.h"
#include <linux/personality.h>
#include <stdbool.h>
#include <ztd_events.h>
#include <ztd_sc_shared.h>
#include <ztd_proc_shared.h>
#include <ztd_shell_shared.h>

#define DEBUG_ENTRY 0

DEFINE_BPF_MAP_HASH_NPA(ppid_cache_map, uint32_t, uint32_t, 4096, AID_SYSTEM);

#if USE_RINGBUF
DEFINE_BPF_RINGBUF(shell_execve_ringbuf, shell_cmd_event_raw_t, RINGBUF_SIZE, AID_ROOT, AID_SYSTEM, 0660);
#endif

static inline __always_inline bool is_session_binary(const char *path) {
    int len = 0;
#pragma unroll 128
    for (int i = 0; i < 128; i++) {
        if (path[i] == '\0') {
            len = i;
            break;
        }
    }
    if (len < 2) return false;

    // Shells (sh, bash, mksh, zsh) and SU
    if (path[len - 1] == 'h' && path[len - 2] == 's') return true;
    // Interpreters (python, node, etc) and wrappers
    if (len >= 6 && path[len - 1] == 'n' && path[len - 2] == 'o') return true;
    if (len > 15 && path[12] == 'a' && path[13] == 'p' && path[14] == 'p') return true;

    return false;
}

static inline __always_inline void updateCmdline(uint64_t args, char cmdline[]) {
    uint64_t argv_addr;
    uint12_t idx = { .value = 0 };
    for (int d = 0; d < MAX_PATH_DEPTH; d++) {
        void *argv_ptr = POINTER_OF_USER_SPACE(args + (d * sizeof(void *)));
        if (!argv_ptr) {
            break;
        }
        bpf_probe_read_user(&argv_addr, sizeof(argv_addr), argv_ptr);
        if (!argv_addr) {
            break;
        }
        if (idx.value > 0 && idx.value < (MAX_FP_LEN - 1)) {
            cmdline[idx.value++] = ' ';
        }
        int ret = 0;
        if (idx.value <= (MAX_FP_LEN - MAX_FN_LEN)) {
            ret = bpf_probe_read_user_str(&cmdline[idx.value], MAX_FN_LEN,
                                          POINTER_OF_USER_SPACE(argv_addr));
        }
        if (ret > 1) {
            idx.value += (ret - 1);
        } else {
            break;
        }
    }
}

static inline __always_inline int onExecveEnter(sys_enter_data_t *args) {
    uint64_t pid_tgid = bpf_get_current_pid_tgid();
    uint32_t tgid_val = (uint32_t) (pid_tgid >> 32);
    uint32_t tid_val = (uint32_t) pid_tgid;

    uint64_t uid_gid = bpf_get_current_uid_gid();
    uint32_t uid_val = (uint32_t) uid_gid;

    char path[128] = {};
    if (args->args[0] != 0) {
        bpf_probe_read_user_str(path, sizeof(path), (void *) args->args[0]);
    }

    uint32_t ppid_val = 0;
    uint32_t * monitored_parent = bpf_ppid_cache_map_lookup_elem(&tid_val);
    if (monitored_parent) ppid_val = *monitored_parent;

    bool session_start = is_session_binary(path);
    bool should_report = false;

    if (uid_val >= FIRST_APP_UID) {
        should_report = true;
    } else if (uid_val == AID_SHELL || uid_val == ROOT_UID) {
        if (monitored_parent || session_start){
            should_report = true;
        }
    }

    if (!should_report) return 1;

    if (!monitored_parent) {
        bpf_ppid_cache_map_update_elem(&tid_val, &ppid_val, BPF_ANY);
    }

    shell_cmd_event_raw_t *data = bpf_shell_execve_ringbuf_reserve();
    if (!data) return 1;

    data->event_time = bpf_ktime_get_boot_ns();
    data->uid = uid_val;
    data->pid = tgid_val;
    data->ppid = ppid_val;
    if (args->args[0] != 0) {
        bpf_probe_read_user_str(data->filepath, sizeof(data->filepath), (void *) args->args[0]);
    }

    updateCmdline(args->args[1], data->cmd);

#if DEBUG_ENTRY
    bpf_printk("[ztdShell-enter] Valid cmd: uid=%u, pid=%d, ppid=%d", data->uid, data->pid, data->ppid);
    bpf_printk("[ztdShell-enter] cmd: cmd=%s", data->cmd);
    bpf_printk("[ztdShell-enter] path: filepath=%s", data->filepath);
#endif

    bpf_shell_execve_ringbuf_submit(data);
    return 1;
}

static inline __always_inline void onProcessExit(shell_sched_process_exit_data_t *args) {
    (void) args;
    uint64_t pid_tgid = bpf_get_current_pid_tgid();
    uint32_t tid_val = (uint32_t) pid_tgid;

    // PRE-FILTERING: Only report EXIT if the PID was being monitored in the map
    if (bpf_ppid_cache_map_delete_elem(&tid_val) < 0) {
        return;
    }

    char comm[16] = {};
    bpf_get_current_comm(&comm, sizeof(comm));

    bool is_shell = false;

    if (comm[0] == 's' && comm[1] == 'h' && comm[2] == '\0') is_shell = true;
    else if (comm[0] == 's' && comm[1] == 'u' && comm[2] == '\0') is_shell = true;
    else if (comm[0] == 'b' && comm[1] == 'a' && comm[2] == 's' && comm[3] == 'h' &&
             comm[4] == '\0')
        is_shell = true;

    if (!is_shell) return;

    shell_cmd_event_raw_t *data = bpf_shell_execve_ringbuf_reserve();
    if (!data) return;

    data->event_time = bpf_ktime_get_boot_ns();
    data->uid = (uint32_t) bpf_get_current_uid_gid();
    data->pid = (uint32_t) (pid_tgid >> 32);
    data->filepath[0] = '<';
    data->filepath[1] = 'E';
    data->filepath[2] = 'X';
    data->filepath[3] = 'I';
    data->filepath[4] = 'T';
    data->filepath[5] = '>';
    data->filepath[6] = '\0';
    data->cmd[0] = '\0';

    bpf_shell_execve_ringbuf_submit(data);
}

DEFINE_SC_TRACEPOINT(raw_syscalls, sys_enter, shell_execve_enter)
(sys_enter_data_t *args) {
    if (args->id == GENERIC_SYSCALL_NR_EXECVE || args->id == ARM32_SYSCALL_NR_EXECVE)
        return onExecveEnter(args);
    RETURN_ALLOW;
}

DEFINE_PROC_TRACEPOINT(sched_process_exit)
(shell_sched_process_exit_data_t *args) {
    onProcessExit(args);
    RETURN_ALLOW;
}

DEFINE_PROC_TRACEPOINT(sched_process_fork)
(shell_sched_process_fork_data_t *args) {
    uint32_t child_pid = (uint32_t)args->child_pid;
    uint32_t parent_pid = (uint32_t)args->parent_pid;
    uint32_t uid_val = (uint32_t)bpf_get_current_uid_gid();

#if DEBUG_ENTRY
    bpf_printk("[ztdShell] sched_process_fork: child_pid=%u, parent_pid=%d", child_pid, parent_pid);
#endif
    if (uid_val >= FIRST_APP_UID || uid_val == ROOT_UID || uid_val == AID_SHELL
        || bpf_ppid_cache_map_lookup_elem(&parent_pid)) {
        bpf_ppid_cache_map_update_elem(&child_pid, &parent_pid, BPF_ANY);
    }

    RETURN_ALLOW;
}

LICENSE("GPL");
