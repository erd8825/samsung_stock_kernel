
#ifndef ZTD_EVENTS_H
#define ZTD_EVENTS_H

#include <ztd_common.h>

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

typedef struct event_ctrl {
    uint64_t cpu_id;
    uint64_t event_cnt;
} event_ctrl_t;

typedef struct event_noti {
    int32_t type;
    uint64_t key;
} event_noti_t;

typedef struct proc_event_raw {
    int32_t event;
    uint64_t event_time;
    int32_t syscall;
    int32_t exit_code;
    pid_t tid;
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t gid;
    uid_t suid;
    gid_t sgid;
    uid_t euid;
    gid_t egid;
    uid_t fsuid;
    gid_t fsgid;
    uid_t owner_uid;
    gid_t owner_gid;
    time64_t atime;
    time64_t mtime;
    time64_t ctime;
    char filepath[MAX_FP_LEN];
    char cwd[MAX_FP_LEN];
    char cmdline[MAX_FP_LEN];
    int64_t reserved_1;
    int64_t reserved_2;
    int64_t reserved_3;
} proc_event_raw_t;

#ifdef __cplusplus
typedef struct proc_event_data {
    int32_t event;
    uint64_t event_time;
    int32_t syscall;
    int32_t exit_code;
    int32_t tid;
    int32_t pid;
    int32_t ppid;
    uint32_t uid;
    uint32_t gid;
    uint32_t suid;
    uint32_t sgid;
    uint32_t euid;
    uint32_t egid;
    uint32_t fsuid;
    uint32_t fsgid;
    uint32_t owner_uid;
    uint32_t owner_gid;
    int64_t atime;
    int64_t mtime;
    int64_t ctime;
    std::string filepath;
    std::string cwd;
    std::string cmdline;
    int64_t reserved_1;
    int64_t reserved_2;
    int64_t reserved_3;
} proc_event_data_t;
#endif

typedef struct sock_event_raw {
    int32_t event;
    uint64_t event_time;
    int32_t syscall;
    int32_t exit_code;
    pid_t tid;
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t gid;
    int32_t oldstate;
    int32_t newstate;
    int32_t fd;
    uint16_t family;
    uint16_t type;
    uint16_t protocol;
    uint16_t sport;
    uint16_t dport;
    uint8_t saddr[4];
    uint8_t daddr[4];
    uint8_t saddr_v6[16];
    uint8_t daddr_v6[16];
    char filepath[MAX_FP_LEN];
} sock_event_raw_t;

#ifdef __cplusplus
typedef struct sock_event_data {
    int32_t event;
    uint64_t event_time;
    int32_t syscall;
    int32_t exit_code;
    pid_t tid;
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t gid;
    int32_t oldstate;
    int32_t newstate;
    int32_t fd;
    uint16_t family;
    uint16_t type;
    uint16_t protocol;
    uint16_t sport;
    uint16_t dport;
    std::string saddr;
    std::string daddr;
    std::string saddr_v6;
    std::string daddr_v6;
    std::string filepath;
} sock_event_data_t;
#endif

typedef struct mdd_event_raw {
    int32_t event;
    uint64_t event_time;
    pid_t pid;
    uid_t uid;
    char filepath[MAX_FP_LEN];
    int signature;
    uint32_t prot;
    int32_t flags;
    uint64_t mapped_addr;
    uint64_t map_size;
    uint32_t load_type;
} mdd_event_raw_t;

#ifdef __cplusplus
typedef struct mdd_event_data {
    int32_t event;
    uint64_t event_time;
    pid_t pid;
    uid_t uid;
    std::string filepath;
    int signature;
    uint32_t prot;
    int32_t flags;
    uint64_t mapped_addr;
    uint64_t map_size;
    uint32_t load_type;
} mdd_event_data_t;

typedef struct load_event_data { 
    int32_t event;
    uint64_t event_time;
    pid_t pid;
    uid_t uid;
    std::string filepath;
    uint32_t prot;
    int32_t flags;
    uint64_t mapped_addr;
    uint64_t map_size;
    uint32_t load_type;
} load_event_data_t;

typedef struct masquerade_event_data { 
    int32_t event;
    uint64_t event_time;
    pid_t pid;
    uid_t uid;
    std::string filepath;
    int signature;
    int32_t flags;
} masquerade_event_data_t;
#endif

typedef struct ioctl_event_raw {
    uid_t uid;
    uint64_t event_time;
    char driver_path[128];
    uint32_t op_code;
    uint64_t counter;
} ioctl_event_raw_t;

#ifdef __cplusplus
typedef struct ioctl_event_data {
    uid_t uid;
    uint64_t event_time;
    std::string driver_path;
    uint32_t op_code;
    uint64_t counter;
} ioctl_event_data_t;
#endif

#endif // ZTD_EVENTS_H