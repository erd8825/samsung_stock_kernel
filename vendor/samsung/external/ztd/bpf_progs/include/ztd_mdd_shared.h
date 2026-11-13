#ifndef ZTD_MDD_SHARED_H
#define ZTD_MDD_SHARED_H

#include <linux/bpf.h>
#include <sys/types.h>

#include <ztd_common.h>

#define PROG_MDD        "ztdMdd"

#define SYS_ENTER_FOR_MDD_PROG_PATH     BPF_FS_PATH PROG_(PROG_MDD)     TRACEPOINT_ "mdd_enter"
#define SYS_EXIT_FOR_MDD_PROG_PATH      BPF_FS_PATH PROG_(PROG_MDD)     TRACEPOINT_ "mdd_exit"

#define MDD_DATA_MAP_PATH           BPF_FS_PATH MAP_(PROG_MDD)        "mdd_data_map"
#define MDD_UIDS_FILTER_MAP_PATH    BPF_FS_PATH MAP_(PROG_MDD)      "mdd_uids_filter_map"

#define BUILD_MDD_EVENT_NOTI_PATH(PROG_NAME)  BPF_FS_PATH MAP_(PROG_NAME)  "mdd_event_noti"

#ifndef MAX_MDD_PREFIX_PATH_LEN
#define MAX_MDD_PREFIX_PATH_LEN 12
#endif

typedef struct mdd_path_filter_data {
    char target[MAX_MDD_PREFIX_PATH_LEN];
} mdd_path_filter_data_t;

typedef struct mdd_path_filter_bytes {
    long high;
    int low;
} mdd_path_filter_bytes_t;

typedef struct mdd_uids_filter_data {
    uid_t uid;
} mdd_uids_filter_data_t;

typedef struct mdd_sc_open_data {
    int fd;
    char filename[MAX_MDD_PREFIX_PATH_LEN];
    int flags;
} mdd_sc_open_data_t;

typedef struct mdd_key_aux_map {
    pid_t pid;
    uid_t uid;
    int fd;
} mdd_key_aux_map_t;

#endif // ZTD_MDD_SHARED_H