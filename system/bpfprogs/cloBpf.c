#include <linux/bpf.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <android_bpf_defs.h>
#include <private/android_filesystem_config.h>
#include "include/cloBpf.h"

#include <linux/pkt_cls.h>
#include <linux/filter.h>

// bionic kernel uapi linux/udp.h header is munged...
#define __kernel_udphdr udphdr
#include <linux/udp.h>

#ifdef CLO_LOG
#include "include/cloBpfLogger.h"
#endif

DEFINE_BPF_MAP_GRO(clo_tx_stats_map, HASH, uint8_t, uint64_t, CLO_TX_STATS_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_stats_map, HASH, uint8_t, CloRxStats, CLO_RX_STATS_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_tx_flow_stats_map, HASH, uint32_t, CloTxFlowStats, CLO_TX_FLOW_STATS_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_flow_rtt_stats_map, HASH, uint32_t, CloRxFlowRttStats, CLO_RX_FLOW_RTT_STATS_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_flow_traffic_stats_map, HASH, uint32_t, CloRxFlowTrafficStats , CLO_RX_FLOW_TRAFFIC_STATS_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_flow_list_tag_map, HASH, uint32_t, CloRxFlowListTag, CLO_RX_FLOW_LIST_TAG_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_flow_list_info_map, HASH, uint8_t, CloRxFlowListInfo, CLO_RX_FLOW_LIST_INFO_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rx_flow_list_map, HASH, uint16_t, uint32_t, CLO_RX_FLOW_LIST_SIZE, AID_SYSTEM)

DEFINE_BPF_MAP_GRO(clo_gro_policy_map, HASH, uint8_t , uint8_t , CLO_GRO_POLICY_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_ho_policy_map, HASH, uint32_t, CloHoPolicy, CLO_HO_POLICY_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_ap_tick_map, HASH, uint8_t, CloApTick, CLO_AP_TICK_SIZE, AID_SYSTEM)

// RTT TRACKING MAPS
DEFINE_BPF_MAP_GRO(clo_tcp_rtt_map, HASH, uint32_t, CloTcpRttData, CLO_RTT_DATA_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_udp_rtt_map, HASH, uint32_t, CloUdpRttData, CLO_RTT_DATA_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_rtt_control_map, ARRAY, uint32_t, CloRttControl, CLO_RTT_CONTROL_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(clo_udp_session_map, LRU_HASH, uint32_t, CloUdpSession, CLO_UDP_SESSION_MAP_SIZE, AID_SYSTEM)

//static int (*bpf_skb_load_bytes)(struct __sk_buff *skb, int off, void *to, int len) = (void *) BPF_FUNC_skb_load_bytes;
//static int (*bpf_l4_csum_replace)(struct __sk_buff *skb, __u32 offset, __u64 from, __u64 to, __u64 flags) = (void *) BPF_FUNC_l4_csum_replace;
//static int (*bpf_skb_store_bytes)(struct __sk_buff *skb, __u32 offset, const void *from, __u32 len, __u64 flags) = (void *) BPF_FUNC_skb_store_bytes;

// Read Non target option
#define CLO_OPT_PASS 0
// Read target option
#define CLO_OPT_SUCCESS 1
// Read target option, but error. 
#define CLO_OPT_FAILURE -1

#define TCP_ACK 0x10
#define TCP_PSH 0x08

#define TCP_FLAG_OFF 13

#define IP_HDR_PROTOCOL  (offsetof(struct iphdr, protocol))
#define IP_HDR_LENGTH    (sizeof(struct iphdr))

#define IPV6_HDR_PROTOCOL (offsetof(struct ipv6hdr, nexthdr))
#define IPV6_HDR_LENGTH   (sizeof(struct ipv6hdr))

#define TCP_HDR_LENGTH    (sizeof(struct tcphdr))
#define TCP_DPORT_OFF     offsetof(struct tcphdr, dest)
#define TCP_SPORT_OFF     offsetof(struct tcphdr, source)
#define TCP_CSUM_OFF      offsetof(struct tcphdr, check)

#define UDP_DPORT_OFF     offsetof(struct udphdr, dest)

// TCP OPTION FIELD
#define TCPOPT_NOP 1
#define TCPOPT_EOL 0
#define TCPOPT_WSCALE 3
#define TCPOPT_TIMESTAMP 8

#define TCPOLEN_WSCALE 3
#define TCPOLEN_TIMESTAMP 10

// TCP DEFINES
#define TCP_MSS 1450
#define TCP_TS_HZ 1000
#define USEC_PER_SEC 1000000L
#define NSEC_PER_USEC 1000L
#define INT_MAX ((int)(~0U >> 1))

// Android only supports little endian architectures
#define htons(x) (__builtin_constant_p(x) ? ___constant_swab16(x) : __builtin_bswap16(x))
#define htonl(x) (__builtin_constant_p(x) ? ___constant_swab32(x) : __builtin_bswap32(x))
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)

#define IP_V4_NW htons(ETH_P_IP)
#define IP_V6_NW htons(ETH_P_IPV6)

#define CLO_RETURN_ERROR -1
#define CLO_RETURN_SUCCESS 0

static __always_inline __attribute__((always_inline))
uint64_t div_u64(uint64_t dividend, uint64_t divisor) {
    return dividend / divisor;
}

static __always_inline __attribute__((always_inline))
uint64_t clo_ktime_get_us() {
    return div_u64(bpf_ktime_get_ns(), NSEC_PER_USEC);
}

static inline __attribute__((always_inline))
void clo_update_rtt_statistics(uint32_t packet_uid, uint32_t est_rtt, uint8_t proto);

// Dynamic Right-Sizing (DRS)
static __always_inline __attribute__((always_inline))
uint32_t clo_moving_average(uint32_t old, uint32_t new) {
    uint32_t new_sample = old;
    long m = new;

    if (new_sample != 0) {
        m -= (new_sample >> 3);
        new_sample += m;
    } else {
        new_sample = m << 3;
    }

    return new_sample;
}

static __always_inline __attribute__((always_inline)) int
clo_get_tcp_wscale_option(struct __sk_buff *skb, uint8_t *fieldlength, uint8_t *ptr, uint8_t *wscale) {
    int ret;
    uint8_t opcode;
    uint8_t opsize;

    if (*fieldlength < 1) return CLO_OPT_FAILURE; // No option field

    ret = bpf_skb_load_bytes(skb, *ptr, &opcode, 1);
    *ptr += 1;

    if (ret) return CLO_OPT_FAILURE; // Load fail
    if (opcode == TCPOPT_EOL) return CLO_OPT_FAILURE; // EOL
    if (opcode == TCPOPT_NOP) { *fieldlength -= 1; return CLO_OPT_PASS; } // NOP

    // TCP Options
    ret = bpf_skb_load_bytes(skb, *ptr, &opsize, 1);
    *ptr += 1;
    if (ret) return CLO_OPT_FAILURE; // Load Fail
    if (opsize < 2 || opsize > *fieldlength) return CLO_OPT_FAILURE; // option size error

    if (opcode == TCPOPT_WSCALE) { //Window Scale factor
        if (opsize == TCPOLEN_WSCALE) {
            uint8_t _wscale;

            ret = bpf_skb_load_bytes(skb, *ptr, &_wscale, 1);
            if (ret) return CLO_OPT_FAILURE;

            *wscale = _wscale;
#ifdef CLO_LOG
            clo_printk("Wscale: wscale(%u)", _wscale);
#endif
        }
        *ptr += opsize - 2;
        *fieldlength -= opsize;
        return CLO_OPT_SUCCESS;
    }

    *ptr += opsize - 2;
    *fieldlength -= opsize;

    return CLO_OPT_PASS;
}

static __always_inline __attribute__((always_inline)) int
clo_get_tcp_timestamp_option(struct __sk_buff *skb, uint8_t *fieldlength, uint8_t *ptr, uint32_t *ts, uint32_t *tsecr) {
    int ret;
    uint8_t opcode;
    uint8_t opsize;

    if (*fieldlength < 1) return CLO_OPT_FAILURE; // No option field

    ret = bpf_skb_load_bytes(skb, *ptr, &opcode, 1);
    *ptr += 1;

    if (ret) return CLO_OPT_FAILURE; // Load fail
    if (opcode == TCPOPT_EOL) return CLO_OPT_FAILURE; // EOL
    if (opcode == TCPOPT_NOP) { *fieldlength -= 1; return CLO_OPT_PASS; } // NOP

    // TCP Options
    ret = bpf_skb_load_bytes(skb, *ptr, &opsize, 1);
    *ptr += 1;
    if (ret) return CLO_OPT_FAILURE; // Load Fail
    if (opsize < 2 || opsize > *fieldlength) return CLO_OPT_FAILURE; // option size error

    if (opcode == TCPOPT_TIMESTAMP) { //Timestamp
        if (opsize == TCPOLEN_TIMESTAMP) {
            uint32_t _ts;
            uint32_t _tsecr;

            ret = bpf_skb_load_bytes(skb, *ptr, &_ts, 4);
            if (ret) return CLO_OPT_FAILURE;
            _ts = ntohl(_ts);

            ret = bpf_skb_load_bytes(skb, *ptr + 4, &_tsecr, 4);
            if (ret) return CLO_OPT_FAILURE;
            _tsecr = ntohl(_tsecr);

            *ts = _ts;
            *tsecr = _tsecr;
#ifdef CLO_LOG
            clo_printk("Timestamp: ts(%u), tsecr(%u)", _ts, _tsecr);
#endif
        }
        *ptr += opsize - 2;
        *fieldlength -= opsize;
        return CLO_OPT_SUCCESS;
    }

    *ptr += opsize - 2;
    *fieldlength -= opsize;

    return CLO_OPT_PASS;
}

// clo get wscale option for syn tx packet
static __always_inline __attribute__((always_inline)) int
clo_try_wscale_option(struct __sk_buff *skb, uint8_t offset, uint8_t *wscale) {
    int ret = 0;
    struct tcphdr th = {0};
    uint8_t field_length;
    uint8_t ptr;

    ret = bpf_skb_load_bytes(skb, offset, &th, TCP_HDR_LENGTH);
    if (ret) return CLO_OPT_FAILURE;

    // Check tcp option fields and timestamp option
    field_length = (th.doff * 4) - TCP_HDR_LENGTH;
    ptr = offset + TCP_HDR_LENGTH;

    // Try to check 6 times in tcp option field.
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;
    ret = clo_get_tcp_wscale_option(skb, &field_length, &ptr, wscale);
    if (ret) return ret;

    return ret;
}

// clo get ts option
static __always_inline __attribute__((always_inline)) int
clo_try_timestamp_option(struct __sk_buff *skb, uint8_t offset, uint32_t *ts, uint32_t *tsecr) {
    int ret = 0;
    struct tcphdr th = {0};
    uint8_t field_length;
    uint8_t ptr;

    ret = bpf_skb_load_bytes(skb, offset, &th, TCP_HDR_LENGTH);
    if (ret) return CLO_OPT_FAILURE;

    // Check tcp option fields and timestamp option
    field_length = (th.doff * 4) - TCP_HDR_LENGTH;
    ptr = offset + TCP_HDR_LENGTH;

    // Try to check 4 times in tcp option field.
    ret = clo_get_tcp_timestamp_option(skb, &field_length, &ptr, ts, tsecr);
    if (ret) return ret;
    ret = clo_get_tcp_timestamp_option(skb, &field_length, &ptr, ts, tsecr);
    if (ret) return ret;
    ret = clo_get_tcp_timestamp_option(skb, &field_length, &ptr, ts, tsecr);
    if (ret) return ret;
    ret = clo_get_tcp_timestamp_option(skb, &field_length, &ptr, ts, tsecr);
    if (ret) return ret;

    return ret;
}

// @Never-Fail
static __always_inline __attribute__((always_inline)) CloRxFlowRttStats
clo_create_rx_flow_rtt_stats(uint16_t sport, uint16_t dport, CloApTick *tick) {
    // init rx_flow
    CloRxFlowRttStats new_rx_stats = {0};

    new_rx_stats.sport = sport;
    new_rx_stats.dport = dport;
    new_rx_stats.rx_syn_tick = tick->tick_num;

    new_rx_stats.rx_rtt_sum_ms = 0;
    new_rx_stats.rx_rtt_sum_count = 0;
    new_rx_stats.rx_rtt_last_tsecr = 0;
    new_rx_stats.rx_rtt_sample_count = 0;
    new_rx_stats.rx_rtt_us = 0;
    new_rx_stats.rx_rtt_min_ms = 0xFFFFFFFF;
    new_rx_stats.rx_rtt_tick_num = 0;

    return new_rx_stats;
}

// @Never-Fail
static __always_inline __attribute__((always_inline)) CloRxFlowTrafficStats
clo_create_rx_flow_traffic_stats(uint16_t sport, uint16_t dport, CloApTick *tick) {
    // init rx_flow
    CloRxFlowTrafficStats new_rx_stats = {0};

    new_rx_stats.sport = sport;
    new_rx_stats.dport = dport;
    new_rx_stats.rx_syn_tick = tick->tick_num;

    new_rx_stats.rx_sum_bytes = 0;
    return new_rx_stats;
}

// @Never-Fail
static __always_inline __attribute__((always_inline)) CloRxFlowListTag
clo_create_rx_flow_list_tag(uint16_t sport, uint16_t dport, CloApTick *tick) {
    // init rx_flow
    CloRxFlowListTag new_rx_tag = {0};

    new_rx_tag.sport = sport;
    new_rx_tag.dport = dport;
    new_rx_tag.rx_syn_tick = tick->tick_num;

    new_rx_tag.rx_list_updated_tick = 0;
    return new_rx_tag;
}

static __always_inline __attribute__((always_inline)) int
clo_create_rx_flow_stats(uint16_t sport, uint16_t dport) {
    uint8_t tick_key = CLO_BPF_MAP_UNIQUE_KEY;
    uint32_t flow_id = CLO_FLOW_ID(sport, dport);
    CloApTick *tick = bpf_clo_ap_tick_map_lookup_elem(&tick_key);

    if (tick) {
        CloRxFlowRttStats cloRxFlowRttStats = clo_create_rx_flow_rtt_stats(sport, dport, tick);
        CloRxFlowTrafficStats cloRxFlowTrafficStats = clo_create_rx_flow_traffic_stats(sport, dport, tick);
        CloRxFlowListTag cloRxFlowListTag = clo_create_rx_flow_list_tag(sport, dport, tick);

        // netd only can read some of them at once, because the stats maps are separated.
        // netd must consider that each stats maps are same flow using rx_syn_tick
        bpf_clo_rx_flow_rtt_stats_map_update_elem(&flow_id, &cloRxFlowRttStats, BPF_ANY);
        bpf_clo_rx_flow_traffic_stats_map_update_elem(&flow_id, &cloRxFlowTrafficStats, BPF_ANY);
        bpf_clo_rx_flow_list_tag_map_update_elem(&flow_id, &cloRxFlowListTag, BPF_ANY);

        return CLO_RETURN_SUCCESS;
    }

    return CLO_RETURN_ERROR;
}

static __always_inline __attribute__((always_inline)) CloTxFlowStats
clo_create_tx_flow_stats(uint16_t sport, uint16_t dport) {
    // init tx_flow
    CloTxFlowStats new_tx_flow_stats = {0};

    new_tx_flow_stats.sport = sport;
    new_tx_flow_stats.dport = dport;
    new_tx_flow_stats.tx_wscale = 0;
    new_tx_flow_stats.tx_tick_num = 0;
    new_tx_flow_stats.tx_pivot_ktime_us = 0;
    new_tx_flow_stats.tx_pivot_ts = 0;
    new_tx_flow_stats.tx_last_rx_bytes = 0;

    return new_tx_flow_stats;
}

static __always_inline __attribute__((always_inline)) void
clo_update_rx_flow_list(CloRxFlowListTag *rx_flow_list_tag) {
    uint8_t tick_key = CLO_BPF_MAP_UNIQUE_KEY;
    uint8_t flow_list_key = 0;
    uint32_t flow_id = CLO_FLOW_ID(rx_flow_list_tag->sport, rx_flow_list_tag->dport);

    CloRxFlowListInfo *list_info;
    CloApTick *tick;

    list_info = bpf_clo_rx_flow_list_info_map_lookup_elem(&flow_list_key);

    // init list_info
    if (!list_info) {
        CloRxFlowListInfo new_list_info = {0};
        new_list_info.last_tick_num = 0;
        new_list_info.flow_count = 0;

        bpf_clo_rx_flow_list_info_map_update_elem(&flow_list_key, &new_list_info, BPF_ANY);
        list_info = bpf_clo_rx_flow_list_info_map_lookup_elem(&flow_list_key);
        if (!list_info) return;
    }

    tick = bpf_clo_ap_tick_map_lookup_elem(&tick_key);
    if (!tick) return;

    if (tick->tick_num - list_info->last_tick_num > 4) { // 5 slots changed
        list_info->flow_count = 0;
        list_info->last_tick_num = tick->tick_num;
#ifdef CLO_LOG
        clo_printk("Ingress:%u list_info cleared", rx_flow_list_tag->dport);
#endif
    }

    if (rx_flow_list_tag->rx_list_updated_tick != list_info->last_tick_num) {
        uint16_t index = list_info->flow_count;

        list_info->flow_count += 1;
        bpf_clo_rx_flow_list_map_update_elem(&index, &flow_id, BPF_ANY);
        bpf_clo_rx_flow_list_info_map_update_elem(&flow_list_key, list_info, BPF_ANY);

        rx_flow_list_tag->rx_list_updated_tick = list_info->last_tick_num;
#ifdef CLO_LOG
        clo_printk("Ingress:%u list_info inserted(%u)", list_info->flow_count);
        clo_printk("Ingress:%u list_info tick updated(%lu)", rx_flow_list_tag->rx_list_updated_tick);
#endif
    }
}

static __always_inline __attribute__((always_inline)) void
clo_update_tx_stats(struct __sk_buff *skb) {
    uint8_t unique_key = CLO_BPF_MAP_UNIQUE_KEY;
    uint64_t *tx_stats;

    tx_stats = bpf_clo_tx_stats_map_lookup_elem(&unique_key);
    if (!tx_stats) {
        uint64_t new_tx_stats = 0;
        bpf_clo_tx_stats_map_update_elem(&unique_key, &new_tx_stats, BPF_ANY);
        tx_stats = bpf_clo_tx_stats_map_lookup_elem(&unique_key);
        if (!tx_stats) return;
    }
    *tx_stats += skb->len;
    bpf_clo_tx_stats_map_update_elem(&unique_key, tx_stats, BPF_ANY);
}

static __always_inline __attribute__((always_inline)) void
clo_update_rx_stats(struct __sk_buff *skb) {
    uint8_t unique_key = CLO_BPF_MAP_UNIQUE_KEY;
    CloRxStats *rx_stats;

    rx_stats = bpf_clo_rx_stats_map_lookup_elem(&unique_key);
    if (!rx_stats) {
        CloRxStats new_rx_stats = {0};

        new_rx_stats.rx_bytes = 0;
        new_rx_stats.rx_count_pkt = 0;
        new_rx_stats.rx_count_psh = 0;

        bpf_clo_rx_stats_map_update_elem(&unique_key, &new_rx_stats, BPF_ANY);
        rx_stats = bpf_clo_rx_stats_map_lookup_elem(&unique_key);
        if (!rx_stats) return;
    }
    rx_stats->rx_bytes += skb->len;
    rx_stats->rx_count_pkt += 1;
    bpf_clo_rx_stats_map_update_elem(&unique_key, rx_stats, BPF_ANY);
}

static __always_inline __attribute__((always_inline)) void
clo_update_rx_flow_traffic_stats(struct __sk_buff *skb, CloRxFlowTrafficStats *rx_flow_traffic_stats) {
    rx_flow_traffic_stats->rx_sum_bytes += skb->len;
}

static __always_inline __attribute__((always_inline)) void
clo_update_rx_flow_rtt(struct __sk_buff *skb, uint8_t offset, CloRxFlowRttStats *rx_stats) {
    uint8_t tick_key = CLO_BPF_MAP_UNIQUE_KEY;
    CloTxFlowStats * tx_stats;
    CloApTick *tick;
    uint16_t dport = rx_stats->dport;
    uint16_t sport = rx_stats->sport;
    uint32_t flow_id = CLO_FLOW_ID(sport, dport);

    // rtt update
    tx_stats = bpf_clo_tx_flow_stats_map_lookup_elem(&flow_id);
#ifdef CLO_LOG
    if (!tx_stats) { clo_printk("Ingress:%u tx_flow null", dport); return; }
#else
    if (!tx_stats) return;
#endif

    tick = bpf_clo_ap_tick_map_lookup_elem(&tick_key);
    if (!tick) return;

    // if min_rtt was measured before ho tick, then reset min rtt
    // refreshed ho tick num must be same to tick_num
    if (tick->ho_tick_num > rx_stats->rx_rtt_tick_num)
        rx_stats->rx_rtt_min_ms = 0xFFFFFFFF;

    if (rx_stats->rx_rtt_tick_num != tick->tick_num) { // slot changed
        // operation per slot
        rx_stats->rx_rtt_tick_num = tick->tick_num;
        rx_stats->rx_rtt_last_tsecr = 0;
        rx_stats->rx_rtt_sample_count = 0;
        rx_stats->rx_rtt_sum_ms += ((rx_stats->rx_rtt_us >> 3) >> 10);
        rx_stats->rx_rtt_sum_count += 1;
#ifdef CLO_LOG
        clo_printk("Ingress:%u slot changed sum rtt %10lu", dport, rx_stats->rx_rtt_sum_ms);
        clo_printk("Ingress:%u slot changed sum count %10lu", dport, rx_stats->rx_rtt_sum_count);
#endif
    }

    if (rx_stats->rx_rtt_sample_count < CLO_RTT_RX_MAX_SAMPLE_COUNT) {
        uint32_t ts = 0;
        uint32_t tsecr = 0;
        uint32_t est_rtt;
        uint64_t ts_current;
        uint32_t packet_uid = tx_stats->uid;

#ifdef CLO_LOG
        clo_printk("RTTStats : Time Stamp Check(packet_uid(%u)", packet_uid);
#endif
        if (clo_try_timestamp_option(skb, offset, &ts, &tsecr) == CLO_OPT_SUCCESS) { // hit timestamp
            uint64_t now_ktime = clo_ktime_get_us();
            ts_current = now_ktime - tx_stats->tx_pivot_ktime_us + tx_stats->tx_pivot_ts * (USEC_PER_SEC / TCP_TS_HZ);
            est_rtt = (uint32_t)(ts_current - tsecr * (USEC_PER_SEC / TCP_TS_HZ)); // us

            if (rx_stats->rx_rtt_last_tsecr != tsecr) {
                uint32_t rtt_ms;

                rx_stats->rx_rtt_us = clo_moving_average(rx_stats->rx_rtt_us, est_rtt);
                rtt_ms = ((rx_stats->rx_rtt_us >> 3) >> 10); // us to ms
                rx_stats->rx_rtt_last_tsecr = tsecr;
                rx_stats->rx_rtt_sample_count += 1;
                if (rx_stats->rx_rtt_min_ms > rtt_ms)
                    rx_stats->rx_rtt_min_ms = rtt_ms;

                clo_update_rtt_statistics(packet_uid, est_rtt, IPPROTO_TCP);
#ifdef CLO_LOG
                clo_printk("Ingress:%u now_ktime(%10lu)", dport, now_ktime);
                clo_printk("Ingress:%u tx_pivot_ktime_us(%10lu)", dport, tx_stats->tx_pivot_ktime_us);
                clo_printk("Ingress:%u tx_pivot_ts(%10lu)", dport, tx_stats->tx_pivot_ts);
                clo_printk("Ingress:%u rx_rtt_last_tsecr(%10lu)", dport, rx_stats->rx_rtt_last_tsecr);
                clo_printk("Ingress:%u ts_current(%10lu)", dport, ts_current);
                clo_printk("Ingress:%u tsecr(%10u)", dport, tsecr);
                clo_printk("Ingress:%u est_rtt(%10u)", dport, est_rtt);
                clo_printk("Ingress:%u sample(%u)", dport, rx_stats->rx_rtt_sample_count);
                clo_printk("Ingress:%u min_rtt_ms: %u updated", dport, rx_stats->rx_rtt_min_ms);
                clo_printk("Ingress:%u rx_rtt_us: %u updated", dport, (rx_stats->rx_rtt_us >> 3));
#endif
            }
        }
    }
}

static __always_inline __attribute__((always_inline)) void
clo_update_tx_adv_wnd(struct __sk_buff *skb, uint8_t offset, CloTxFlowStats *tx_stats) {
    int ret = 0;
    uint8_t flag;
    uint16_t sport = tx_stats->sport;
    uint16_t dport = tx_stats->dport;
    uint32_t flow_id = CLO_FLOW_ID(dport, sport);
    CloHoPolicy *ho_policy;

    ho_policy = bpf_clo_ho_policy_map_lookup_elem(&flow_id);
    if (!ho_policy) { // handover policy deactivated
        CloRxFlowTrafficStats * rx_traffic_stats = bpf_clo_rx_flow_traffic_stats_map_lookup_elem(&flow_id);
        if (!rx_traffic_stats) return;

        tx_stats->tx_last_rx_bytes = rx_traffic_stats->rx_sum_bytes;
        return;
    }

    //handover policy activated
    ret = bpf_skb_load_bytes(skb, offset + TCP_FLAG_OFF, &flag, 1);
    if (ret) return;
    if (flag & TCP_ACK) {
        uint16_t awnd_n = 0;

        ret = bpf_skb_load_bytes(skb, offset + TCP_FLAG_OFF + 1, &awnd_n, 2);
        if (!ret) {
            uint16_t target_awnd = ho_policy->target_window;
            uint16_t awnd_h = ntohs(awnd_n);

#ifdef CLO_LOG
            clo_printk("Egress:%u awnd: %u", sport, awnd_h);
            clo_printk("Egress:%u t_awnd: %u", sport, target_awnd);
#endif
            // replace awnd
            if (target_awnd < awnd_h) {
                uint16_t new_awnd = htons(target_awnd);

                ret = bpf_l4_csum_replace(skb, offset + TCP_CSUM_OFF, awnd_n, new_awnd, sizeof(new_awnd));
                if (ret) return;
                bpf_skb_store_bytes(skb, offset + TCP_FLAG_OFF + 1, &new_awnd, sizeof(new_awnd), 0);
#ifdef CLO_LOG
                clo_printk("Egress:%u awnd replaced: %u", sport, target_awnd);
#endif
            }
        }
    }
}

static __always_inline __attribute__((always_inline)) void
clo_update_rtt_tx_pivot_timestamp(struct __sk_buff *skb, uint8_t offset, CloTxFlowStats *tx_stats) {
    uint8_t tick_key = CLO_BPF_MAP_UNIQUE_KEY;
#ifdef CLO_LOG
    uint16_t sport = tx_stats->sport;
#endif
    CloApTick *tick;

    tick = bpf_clo_ap_tick_map_lookup_elem(&tick_key);
#ifdef CLO_LOG
    if (!tick) { clo_printk("Egress:%u no ap tick", sport); return; }
#else
    if (!tick) return;
#endif

    if (tx_stats->tx_tick_num != tick->tick_num) { // slot changed.
        uint64_t now_ktime = clo_ktime_get_us();

        // Need to check pivot
        if (now_ktime < tx_stats->tx_pivot_ktime_us + CLO_RTT_TX_PIVOT_EXPIRE_INTERVAL_US) {
            // valid pivot then reuse pivot
            tx_stats->tx_tick_num = tick->tick_num;
#ifdef CLO_LOG
            clo_printk("Egress:%u tx tick %lu updated", sport, tx_stats->tx_tick_num);
#endif
        } else { // different port or invalid pivot
            uint32_t ts = 0;
            uint32_t tsecr = 0;

            if (clo_try_timestamp_option(skb, offset, &ts, &tsecr) == CLO_OPT_SUCCESS) { // hit timestamp!
                tx_stats->tx_tick_num = tick->tick_num;
                tx_stats->tx_pivot_ktime_us = now_ktime;
                tx_stats->tx_pivot_ts = (uint64_t) ts;
#ifdef CLO_LOG
                clo_printk("Egress:%u tx tick %lu fully updated", sport, tx_stats->tx_tick_num);
            } else {
                clo_printk("Egress:%u tx tick %lu fail to fully update", sport, tx_stats->tx_tick_num);
            }
#else
            }
#endif
        }
    }
}

static __always_inline void clo_inc_hist(uint32_t *arr, __u32 idx)
{
#pragma unroll
    for (__u32 i = 0; i < CLO_RTT_HISTOGRAM_SIZE; i++) {
        if (idx == i) {
            arr[i]++;
            return;
        }
    }
}

static inline __attribute__((always_inline))
void clo_update_rtt_statistics(uint32_t packet_uid, uint32_t est_rtt, uint8_t proto)
{
    uint32_t control_key = CLO_RTT_CONTROL_STATIC_KEY;
    CloRttControl *control = bpf_clo_rtt_control_map_lookup_elem(&control_key);
    if (!control || !control->is_enabled) {
        return;
    }

#ifdef CLO_LOG
    clo_printk("RTTStats : RTT Control Check: enabled=%u, uid=0x%x, uid=0x%x",
               control->is_enabled, control->uid, packet_uid);
#endif

    __u32 rtt_key = 0;
    if (control->uid == 0) {
        rtt_key = packet_uid;
    } else {
        if (packet_uid != control->uid) {
#ifdef CLO_LOG
            clo_printk("RTTStats : Uid is different from target uid");
#endif
            return;
        }
        rtt_key = control->uid;
    }

    CloRttStats *v = NULL;

    if (proto == IPPROTO_TCP) {
        CloTcpRttData *tcp_data = bpf_clo_tcp_rtt_map_lookup_elem(&rtt_key);
        if (tcp_data) v = &tcp_data->s;
    }

    if (proto == IPPROTO_UDP) {
        CloUdpRttData *udp_data = bpf_clo_udp_rtt_map_lookup_elem(&rtt_key);
        if (udp_data) v = &udp_data->s;
    }

    if (!v) {
        CloRttStats init = {};
        init.rtt_min_us          = 0xFFFFFFFF;
        init.last_update_time_us = clo_ktime_get_us();

        if (proto == IPPROTO_TCP) {
            CloTcpRttData init_wrapper = { .s = init };
            bpf_clo_tcp_rtt_map_update_elem(&rtt_key, &init_wrapper, BPF_ANY);
            CloTcpRttData *tcp_v = bpf_clo_tcp_rtt_map_lookup_elem(&rtt_key);
            if (tcp_v) v = &tcp_v->s;
        } else if (proto == IPPROTO_UDP) {
            CloUdpRttData init_wrapper = { .s = init };
            bpf_clo_udp_rtt_map_update_elem(&rtt_key, &init_wrapper, BPF_ANY);
            CloUdpRttData *udp_v = bpf_clo_udp_rtt_map_lookup_elem(&rtt_key);
            if (udp_v) v = &udp_v->s;
        }

        if (!v) {
            return;
        }
    }

    __u32 ms_hist_index = CLO_RTT_HISTOGRAM_SIZE;
    __u32 sec_hist_index = CLO_RTT_HISTOGRAM_SIZE;

    if (est_rtt < 10 * CLO_SLOT_US) {
        __u32 histogram_index = est_rtt / CLO_SLOT_US;
        ms_hist_index = histogram_index;

        clo_inc_hist(v->rtt_ms_hist, ms_hist_index);

    } else if (est_rtt < 110 * CLO_SLOT_US) {
        __u32 histogram_index = est_rtt / (CLO_SLOT_US * 10);
        sec_hist_index = (histogram_index > 0) ? (histogram_index - 1) : 0;

        clo_inc_hist(v->rtt_sec_hist, sec_hist_index);

    } else if (est_rtt < 600 * CLO_SLOT_US) {
        v->long_rtt_cnt++;
    }

#ifdef CLO_LOG
    if (ms_hist_index < CLO_RTT_HISTOGRAM_SIZE) {
        clo_printk("RTTStats ::: Hist : uid=%u, rtt=%u us, ms_idx=%u",
                rtt_key, est_rtt, ms_hist_index);
    } else if (sec_hist_index < CLO_RTT_HISTOGRAM_SIZE) {
        clo_printk("RTTStats ::: Hist : uid=%u, rtt=%u us, sec_idx=%u",
                rtt_key, est_rtt, sec_hist_index);
    } else {
        clo_printk("RTTStats : RTT Long RTT: uid=%u, rtt=%u us",
                rtt_key, est_rtt);
    }
#endif

    v->total_samples++;
    v->last_update_time_us = clo_ktime_get_us();

    if (v->total_samples == 1) {
        v->rtt_avg_us = est_rtt;
        v->rtt_min_us = est_rtt;
        v->rtt_max_us = est_rtt;
    } else {
        /* running average */
        v->rtt_avg_us = (uint32_t)((((uint64_t)v->rtt_avg_us *
                                               (v->total_samples - 1)) + est_rtt)
        / v->total_samples);
        if (est_rtt < v->rtt_min_us)
            v->rtt_min_us = est_rtt;
        if (est_rtt > v->rtt_max_us)
            v->rtt_max_us = est_rtt;
    }

#ifdef CLO_LOG
    clo_printk("RTTStats : RTT-HIST: avg=%u min=%u max=%u",
               v->rtt_avg_us, v->rtt_min_us, v->rtt_max_us);

    clo_printk("RTTStats : RTT-HIST: total=%llu",
               v->total_samples);
#endif
}

static inline __attribute__((always_inline)) void
clo_handle_udp_ingress(uint32_t flow_id) {
    uint64_t now = bpf_ktime_get_ns();
    CloUdpSession *session;

    session = bpf_clo_udp_session_map_lookup_elem(&flow_id);
    if (!session) return;

    if (session->is_measuring) {
        uint64_t rtt_ns = now - session->burst_start_time_ns;
        uint32_t rtt_us = rtt_ns >> 10;

        if (rtt_us < CLO_UDP_RTT_MAX_US) {
            clo_update_rtt_statistics(session->uid, rtt_us, IPPROTO_UDP);

#ifdef CLO_LOG
            clo_printk("[%u] RTT Measured! rtt=%u us (uid=%u)", flow_id, rtt_us, session->uid);
#endif
        }
        session->is_measuring = 0;
    }
    session->last_rx_time_ns = now;
}

// Read Timestamp tcp option and Read RX bytes
DEFINE_BPF_PROG("schedcls/ingress/CloIngress", AID_ROOT, AID_SYSTEM, schedcls_ingress_CloIngress)
(struct __sk_buff *skb) {
    int ret = -1;
    uint8_t proto = 0;
    uint8_t offset = IP_HDR_LENGTH;

    // Fetch the port
    if (skb->protocol == IP_V4_NW) {
        ret = bpf_skb_load_bytes(skb, IP_HDR_PROTOCOL, &proto, 1);
    } else if (skb->protocol == IP_V6_NW) {
        ret = bpf_skb_load_bytes(skb, IPV6_HDR_PROTOCOL, &proto, 1);
        offset = IPV6_HDR_LENGTH;
    }

    if (ret) return TC_ACT_UNSPEC;

    clo_update_rx_stats(skb);

    if (proto == IPPROTO_TCP) {
        struct tcphdr th = {0};
        uint16_t sport = 0;
        uint16_t dport = 0;
        uint32_t flow_id = 0;
        CloRxFlowRttStats *rx_flow_rtt_stats;
        CloRxFlowTrafficStats *rx_flow_traffic_stats;
        CloRxFlowListTag *rx_flow_list_tag;

        // Fetch the ports
        ret = bpf_skb_load_bytes(skb, offset + TCP_SPORT_OFF, &sport, 2);
        if (ret) return TC_ACT_UNSPEC;
        ret = bpf_skb_load_bytes(skb, offset + TCP_DPORT_OFF, &dport, 2);
        if (ret) return TC_ACT_UNSPEC;

        flow_id = CLO_FLOW_ID(sport, dport);

        ret = bpf_skb_load_bytes(skb, offset, &th, TCP_HDR_LENGTH);
        if (ret) return TC_ACT_UNSPEC;

        if (th.syn) {
            ret = clo_create_rx_flow_stats(sport, dport);
#ifdef CLO_LOG
            if (ret) clo_printk("CloIngress:%u fail to create rxFlowStats", dport);
#endif
            if (ret) return TC_ACT_UNSPEC;
        }

        rx_flow_rtt_stats = bpf_clo_rx_flow_rtt_stats_map_lookup_elem(&flow_id);
        if (!rx_flow_rtt_stats) return TC_ACT_OK;
        rx_flow_traffic_stats = bpf_clo_rx_flow_traffic_stats_map_lookup_elem(&flow_id);
        if (!rx_flow_traffic_stats) return TC_ACT_OK;
        rx_flow_list_tag = bpf_clo_rx_flow_list_tag_map_lookup_elem(&flow_id);
        if (!rx_flow_list_tag) return TC_ACT_OK;

        clo_update_rx_flow_traffic_stats(skb, rx_flow_traffic_stats);
        clo_update_rx_flow_rtt(skb, offset, rx_flow_rtt_stats);
        clo_update_rx_flow_list(rx_flow_list_tag);

        bpf_clo_rx_flow_rtt_stats_map_update_elem(&flow_id, rx_flow_rtt_stats, BPF_ANY);
        bpf_clo_rx_flow_traffic_stats_map_update_elem(&flow_id, rx_flow_traffic_stats, BPF_ANY);
        bpf_clo_rx_flow_list_tag_map_update_elem(&flow_id, rx_flow_list_tag, BPF_ANY);
    } else if (proto == IPPROTO_UDP) {
        uint16_t sport = 0;
        uint16_t dport = 0;
        uint32_t flow_id = 0;

        ret = bpf_skb_load_bytes(skb, offset + UDP_DPORT_OFF, &dport, 2);
        if (ret) return TC_ACT_UNSPEC;
        ret = bpf_skb_load_bytes(skb, offset + offsetof(struct udphdr, source), &sport, 2);
        if (ret) return TC_ACT_UNSPEC;

        flow_id = CLO_FLOW_ID(sport, dport);

#ifdef CLO_LOG
        clo_printk("[%u] UDP Ingress : len=%d", flow_id, skb->len);
#endif
        clo_handle_udp_ingress(flow_id);
    }

    return TC_ACT_OK;
}

static inline __attribute__((always_inline)) void
clo_handle_udp_egress(struct __sk_buff *skb, uint32_t flow_id) {
    uint64_t now = bpf_ktime_get_ns();
    uint32_t packet_uid = bpf_get_socket_uid(skb);
    CloUdpSession *session;
    CloUdpSession new_session = {0};
    uint32_t control_key = CLO_RTT_CONTROL_STATIC_KEY;

    CloRttControl *control = bpf_clo_rtt_control_map_lookup_elem(&control_key);
    if (!control || !control->is_enabled) {
        return;
    }

    if (control->uid != packet_uid) {
        return;
    }

#ifdef CLO_LOG
    clo_printk("[%u] UDP Egress : len=%d", flow_id, skb->len);
#endif
    session = bpf_clo_udp_session_map_lookup_elem(&flow_id);
    if (!session) {
        new_session.last_tx_time_ns = now;
        new_session.last_rx_time_ns = now;
        new_session.uid = bpf_get_socket_uid(skb);

        int ret = bpf_clo_udp_session_map_update_elem(&flow_id, &new_session, BPF_ANY);
        if (ret) {
#ifdef CLO_LOG
			clo_printk("[%u] Failed to create UDP session ret=%d", flow_id, ret);
#endif
            return;
        }
#ifdef CLO_LOG
        clo_printk("[%u] New Session Created", flow_id);
#endif		
        return; 
    }

    uint64_t time_since_last_tx = now - session->last_tx_time_ns;
    uint64_t time_since_last_rx = now - session->last_rx_time_ns;

    if (time_since_last_tx > CLO_UDP_IDLE_THRESHOLD_NS &&
        time_since_last_rx > CLO_UDP_IDLE_THRESHOLD_NS) {

		session->burst_start_time_ns = now;
		session->is_measuring = 1;
		session->uid = bpf_get_socket_uid(skb);

#ifdef CLO_LOG
		clo_printk("[%u] New Chunk Detect! len=%d, gap_tx=%llu ms", 
					flow_id, skb->len, time_since_last_tx / 1000000);
#endif
    }
    session->last_tx_time_ns = now;
}

// Read and Update advertise window
DEFINE_BPF_PROG("schedcls/egress/CloEgress", AID_ROOT, AID_SYSTEM, schedcls_egress_CloEgress)
(struct __sk_buff *skb) {
    int ret = -1;
    uint8_t proto = 0;
    uint16_t sport = 0;
    uint16_t dport = 0;
    uint8_t offset = IP_HDR_LENGTH;

    // Fetch the port
    if (skb->protocol == IP_V4_NW) {
        ret = bpf_skb_load_bytes(skb, IP_HDR_PROTOCOL, &proto, 1);
    } else if (skb->protocol == IP_V6_NW) {
        ret = bpf_skb_load_bytes(skb, IPV6_HDR_PROTOCOL, &proto, 1);
        offset = IPV6_HDR_LENGTH;
    }

    if (ret) return TC_ACT_UNSPEC;

    clo_update_tx_stats(skb);

    if (proto == IPPROTO_TCP) {
        struct tcphdr th = {0};
        CloTxFlowStats *tx_flow;
        uint32_t flow_id = 0;

        ret = bpf_skb_load_bytes(skb, offset + TCP_SPORT_OFF, &sport, 2);
        if (ret) return TC_ACT_UNSPEC;
        ret = bpf_skb_load_bytes(skb, offset + TCP_DPORT_OFF, &dport, 2);
        if (ret) return TC_ACT_UNSPEC;

        flow_id = CLO_FLOW_ID(dport, sport);

        ret = bpf_skb_load_bytes(skb, offset, &th, TCP_HDR_LENGTH);
        if (ret) return TC_ACT_UNSPEC;

        if (th.syn) {
            uint8_t wscale = 0;
            CloTxFlowStats new_tx_flow_stats = clo_create_tx_flow_stats(sport, dport);

            uint32_t uid = bpf_get_socket_uid(skb);
            new_tx_flow_stats.uid = uid;

            if (clo_try_wscale_option(skb, offset, &wscale) == CLO_OPT_SUCCESS)
                new_tx_flow_stats.tx_wscale = wscale;
#ifdef CLO_LOG
            clo_printk("Egress:%u tx wscale: %u", sport, new_tx_flow_stats.tx_wscale);
#endif
            bpf_clo_tx_flow_stats_map_update_elem(&flow_id, &new_tx_flow_stats, BPF_ANY);
        }

        tx_flow = bpf_clo_tx_flow_stats_map_lookup_elem(&flow_id);
        if (!tx_flow) return TC_ACT_OK;

        clo_update_rtt_tx_pivot_timestamp(skb, offset, tx_flow);
        clo_update_tx_adv_wnd(skb, offset, tx_flow);

        bpf_clo_tx_flow_stats_map_update_elem(&flow_id, tx_flow, BPF_ANY);

    } else if (proto == IPPROTO_UDP) {
        uint16_t sport = 0;
        uint16_t dport = 0;
        uint32_t flow_id = 0;

        ret = bpf_skb_load_bytes(skb, offset + offsetof(struct udphdr, source), &sport, 2);
        if (ret) return TC_ACT_UNSPEC;
        ret = bpf_skb_load_bytes(skb, offset + UDP_DPORT_OFF, &dport, 2);
        if (ret) return TC_ACT_UNSPEC;

        flow_id = CLO_FLOW_ID(dport, sport);

        clo_handle_udp_egress(skb, flow_id);
    }
    return TC_ACT_OK;
}

LICENSE("Apache 2.0");
CRITICAL("Advanced CP MX CLO");