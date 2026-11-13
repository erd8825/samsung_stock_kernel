#pragma once

// This header file is shared by eBPF kernel programs (C) and netd (C++) and
// some of the maps are also accessed directly from Java mainline module code.
//
// Hence: explicitly pad all relevant structures and assert that their size
// is the sum of the sizes of their fields.
#define STRUCT_SIZE(name, size) _Static_assert(sizeof(name) == (size), "Incorrect struct size.")

#define CLO_BPF_PATH "/sys/fs/bpf/"
#define CLO_BPF_NETD_PATH "/sys/fs/bpf/netd_shared/"
// PATH - PROGRAM
#define PATH_CLO_INGRESS_PROG CLO_BPF_NETD_PATH "prog_cloBpf_schedcls_ingress_CloIngress"
#define PATH_CLO_EGRESS_PROG CLO_BPF_NETD_PATH "prog_cloBpf_schedcls_egress_CloEgress"

// PATH - MAP
#define PATH_CLO_RX_STATS_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_stats_map"
#define PATH_CLO_TX_STATS_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_tx_stats_map"
#define PATH_CLO_RX_FLOW_RTT_STATS_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_flow_rtt_stats_map"
#define PATH_CLO_RX_FLOW_TRAFFIC_STATS_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_flow_traffic_stats_map"
#define PATH_CLO_TX_FLOW_STATS_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_tx_flow_stats_map"
#define PATH_CLO_RX_FLOW_LIST_TAG_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_flow_list_tag_map"
#define PATH_CLO_RX_FLOW_LIST_INFO_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_flow_list_info_map"
#define PATH_CLO_RX_FLOW_LIST CLO_BPF_NETD_PATH "map_cloBpf_clo_rx_flow_list_map"

#define PATH_CLO_GRO_POLICY_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_gro_policy_map"
#define PATH_CLO_HO_POLICY_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_ho_policy_map"
#define PATH_CLO_AP_TICK_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_ap_tick_map"

// RTT TRACKING MAPS
#define PATH_CLO_TCP_RTT_DATA_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_tcp_rtt_map"
#define PATH_CLO_UDP_RTT_DATA_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_udp_rtt_map"
#define PATH_CLO_RTT_CONTROL_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_rtt_control_map"
#define PATH_CLO_UDP_SESSION_MAP CLO_BPF_NETD_PATH "map_cloBpf_clo_udp_session_map"

// RTT MEASUREMENT CONFIGURATION
#define CLO_RTT_TX_PIVOT_EXPIRE_INTERVAL_US 5000000L
#define CLO_RTT_RX_MAX_SAMPLE_COUNT 8
#define CLO_RTT_CONTROL_STATIC_KEY 0
#define CLO_UDP_IDLE_THRESHOLD_NS 100000000ULL

#define CLO_GRO_POLICY_MAP_SIZE (1 << 0)

#define CLO_HO_POLICY_MAP_SIZE (1 << 16)
typedef struct {
    uint16_t target_window;
} CloHoPolicy;
STRUCT_SIZE(CloHoPolicy, sizeof(CloHoPolicy));

#define CLO_TX_STATS_MAP_SIZE (1 << 0)

#define CLO_RX_STATS_MAP_SIZE (1 << 0)
typedef struct {
    uint64_t rx_bytes;
    uint64_t rx_count_pkt;
    uint64_t rx_count_psh;
} CloRxStats;
STRUCT_SIZE(CloRxStats, sizeof(CloRxStats));

#define CLO_TX_FLOW_STATS_MAP_SIZE (1 << 16)
typedef struct {
    uint16_t sport;
    uint16_t dport;  // Add dport for better flow tracking
    uint32_t uid;
    uint8_t tx_wscale;
    uint64_t tx_tick_num;
    uint64_t tx_pivot_ktime_us;
    uint64_t tx_pivot_ts;

    uint64_t tx_last_rx_bytes;
} CloTxFlowStats;
STRUCT_SIZE(CloTxFlowStats, sizeof(CloTxFlowStats));

#define CLO_RX_FLOW_RTT_STATS_MAP_SIZE (1 << 16)
typedef struct {
    // RTT
    uint64_t rx_rtt_sum_ms;
    uint64_t rx_rtt_sum_count;
    uint64_t rx_rtt_last_tsecr;
    uint32_t rx_rtt_us;
    uint32_t rx_rtt_min_ms;
    uint64_t rx_rtt_tick_num;
    uint8_t  rx_rtt_sample_count;
	//struct packing per 32 bits
    uint16_t sport;  // Add sport for better flow tracking
    uint16_t dport;
    uint64_t rx_syn_tick;
} CloRxFlowRttStats;
STRUCT_SIZE(CloRxFlowRttStats, sizeof(CloRxFlowRttStats));

#define CLO_RX_FLOW_TRAFFIC_STATS_MAP_SIZE (1 << 16)
typedef struct {
    uint16_t sport;  // Add sport for better flow tracking
    uint16_t dport;
    uint64_t rx_syn_tick;

    // BYTES
    uint64_t rx_sum_bytes;
} CloRxFlowTrafficStats;
STRUCT_SIZE(CloRxFlowTrafficStats, sizeof(CloRxFlowTrafficStats));

#define CLO_RX_FLOW_LIST_TAG_SIZE (1 << 16)
typedef struct {
    uint16_t sport;  // Add sport for better flow tracking
    uint16_t dport;
    uint64_t rx_syn_tick;

    // LIST
    uint64_t rx_list_updated_tick;
} CloRxFlowListTag;
STRUCT_SIZE(CloRxFlowListTag, sizeof(CloRxFlowListTag));

#define CLO_RX_FLOW_LIST_SIZE (1 << 16)
#define CLO_RX_FLOW_LIST_INFO_SIZE (1 << 0)
typedef struct {
    uint64_t last_tick_num;
    uint16_t flow_count;
} CloRxFlowListInfo;
STRUCT_SIZE(CloRxFlowListInfo, sizeof(CloRxFlowListInfo));

#define CLO_BPF_MAP_UNIQUE_KEY 0
#define CLO_AP_MONITOR_STATS_MAP_SIZE (1 << 0)

#define CLO_AP_TICK_SIZE (1 << 0)
typedef struct {
    uint64_t tick_us;
    uint64_t tick_num;
    uint64_t ho_tick_num;
} CloApTick;
STRUCT_SIZE(CloApTick, sizeof(CloApTick));

// RTT TRACKING STRUCTURES
#define CLO_RTT_HISTOGRAM_SIZE 10
#define CLO_SLOT_US 100000u   // 100ms
#define CLO_RTT_DATA_MAP_SIZE (1 << 3)
typedef struct {
    uint32_t rtt_ms_hist[CLO_RTT_HISTOGRAM_SIZE];
    uint32_t rtt_sec_hist[CLO_RTT_HISTOGRAM_SIZE];

    // RTT Statistics
    uint32_t long_rtt_cnt;
    uint32_t rtt_avg_us;
    uint32_t rtt_min_us;
    uint32_t rtt_max_us;
    uint64_t total_samples;
    uint64_t last_update_time_us;
} CloRttStats;
STRUCT_SIZE(CloRttStats, sizeof(CloRttStats));

typedef struct {CloRttStats s;} CloTcpRttData;
STRUCT_SIZE(CloTcpRttData, sizeof(CloTcpRttData));

typedef struct {CloRttStats s;} CloUdpRttData;
STRUCT_SIZE(CloUdpRttData, sizeof(CloUdpRttData));

#define CLO_RTT_CONTROL_MAP_SIZE 1
typedef struct {
    uint8_t is_enabled;
    uint32_t uid;
    uint64_t start_time_us;
    uint64_t total_samples;
} CloRttControl;
STRUCT_SIZE(CloRttControl, sizeof(CloRttControl));

#define CLO_UDP_SESSION_MAP_SIZE (1 << 10)
#define CLO_UDP_RTT_MAX_US 10000000u
typedef struct {
    uint64_t last_tx_time_ns;
    uint64_t last_rx_time_ns;
    uint64_t burst_start_time_ns;
    uint32_t uid;
    uint32_t is_measuring;
} CloUdpSession;
STRUCT_SIZE(CloUdpSession, sizeof(CloUdpSession));

#define CLO_FLOW_ID(remote_port, device_port) ((((uint32_t)(remote_port) << 16) | (device_port)))
#define CLO_FLOW_REMOTE_PORT(hash) ((uint16_t)((hash) >> 16))
#define CLO_FLOW_DEVICE_PORT(hash) ((uint16_t)((hash) & 0xFFFF))
#undef STRUCT_SIZE