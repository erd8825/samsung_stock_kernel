#ifndef ZTD_SHELL_SHARED_H
#define ZTD_SHELL_SHARED_H

#include <sys/types.h>
#include <ztd_common.h>
#include <ztd_proc_shared.h>

#define PROG_SHELL_EXECVE "ztdShell"

#define SYS_SHELL_ENTER_FOR_EXECVE_PROG_PATH \
    BPF_FS_PATH PROG_(PROG_SHELL_EXECVE) TRACEPOINT_ "shell_execve_enter"

#define SYS_SHELL_EXIT_FOR_EXECVE_PROG_PATH \
    BPF_FS_PATH PROG_(PROG_SHELL_EXECVE) TRACEPOINT_ "shell_execve_exit"

#define SHELL_SCHED_PROCESS_FORK_PROG_PATH \
    BPF_FS_PATH PROG_(PROG_SHELL_EXECVE) TRACEPOINT_ "sched_process_fork"

#define SHELL_EXECVE_RINGBUF_PATH   BPF_FS_PATH MAP_(PROG_SHELL_EXECVE)  "shell_execve_ringbuf"

#define RINGBUF_SIZE (2 * 1024 * 1024) // 2 MB
#define MAX_LEN 4096
#define MAX_PATH_DEPTH 128
#define AID_SHELL 2000
#define FIRST_APP_UID 10000
#define ROOT_UID 0

typedef struct {
    uint64_t event_time;
    uint32_t uid;
    uint32_t pid;
    uint32_t ppid;
    char filepath[MAX_LEN];
    char cmd[MAX_LEN];
} __attribute__((packed)) shell_cmd_event_raw_t;

#ifdef __cplusplus
typedef struct shell_cmd_event_data {
    uint64_t event_time;
    uint32_t uid;
    uint32_t pid;
    uint32_t ppid;
    std::string filepath;
    std::string cmd;
} shell_cmd_event_data_t;
#endif

typedef struct {
    uint64_t common;
    char comm[MAX_TASK_COMM_LEN];
    int32_t pid;
    int32_t prio;
} shell_sched_process_exit_data_t;

typedef struct {
    uint64_t common;
    char parent_comm[16];
    int32_t parent_pid;
    char child_comm[16];
    int32_t child_pid;
} shell_sched_process_fork_data_t;

#endif // ZTD_SHELL_SHARED_H
