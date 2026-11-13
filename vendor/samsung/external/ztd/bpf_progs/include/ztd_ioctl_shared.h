#ifndef ZTD_IOCTL_MONITORING_H
#define ZTD_IOCTL_MONITORING_H

#include <linux/bpf.h>
#include <sys/types.h>

#include <ztd_common.h>

#ifdef __cplusplus
#include <string>
#endif

#define PROG_IOCTL  "ztdScIoctl"

#define IOCTL_UIDS_FILTER_MAP_PATH  BPF_FS_PATH MAP_(PROG_IOCTL) "ioctl_uids_filter_map"
#define IOCTL_PATHS_FILTER_MAP_PATH BPF_FS_PATH MAP_(PROG_IOCTL) "ioctl_paths_filter_map"

typedef struct ioctl_sc_open_data {
    int fd;
    char filename[128];
} ioctl_sc_open_data_t;

typedef struct ioctl_key_aux_map {
    uid_t uid;
    int fd;
} ioctl_key_aux_map_t;

typedef struct ioctl_key_map {
    uid_t uid;
    int fd;
    uint32_t op_code;
    char driver_path[128];
} ioctl_key_map_t;

typedef struct ioctl_path_bytes {
    long data[8];
} ioctl_path_bytes_t;

#endif // ZTD_IOCTL_MONITORING_H