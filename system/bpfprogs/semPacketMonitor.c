#include <linux/bpf.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <android_bpf_defs.h>

#include <linux/pkt_cls.h>
#include <linux/filter.h>

// bionic kernel uapi linux/udp.h header is munged...
#define __kernel_udphdr udphdr
#include <linux/udp.h>

#include <ss_bpf_shared.h>

// This is used for xt_bpf program only.
#define BPF_NOMATCH 0

#define TCP_FLAG_OFF      13
#define TCP_DOFF_OFF      12

#define IP_OFF_SRC   (offsetof(struct iphdr, saddr))
#define IP_OFF_DST   (offsetof(struct iphdr, daddr))

// Note: bpf_skb_load_bytes and bpf_get_socket_uid are already defined in android_bpf_defs.h

#define TCP_DPORT_OFF     offsetof(struct tcphdr, dest)
#define TCP_SPORT_OFF     offsetof(struct tcphdr, source)

static __always_inline void update_zero_window(SessionInfo *info, uint16_t raw_window, bool is_client_side) {
    if (raw_window != 0) {
        return;
    }

    if (is_client_side) {
        if (info->zero_window_count_client < 0xFFFFFFFFu) {
            info->zero_window_count_client += 1;
        }
    } else {
        if (info->zero_window_count_server < 0xFFFFFFFFu) {
            info->zero_window_count_server += 1;
        }
    }
}

// Android only supports little endian architectures
#define htons(x) (__builtin_constant_p(x) ? ___constant_swab16(x) : __builtin_bswap16(x))
#define htonl(x) (__builtin_constant_p(x) ? ___constant_swab32(x) : __builtin_bswap32(x))
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)

// PacketMonitor Map Size Definition - Based on the limited five FG apps, optimize
#define PACKETMONITOR_UID_MAP_SIZE 8               // UID Map: 8 (FG 5개 + Extra)
#define PACKETMONITOR_DEST6_MAP_SIZE 1024          // Port→UID Map: 1K (Entries are organized through LRU)
#define PACKETMONITOR_UID_PORT_MAP_SIZE 1024       // UID-Port Map: 1K (Entries are organized through LRU)
#define PACKETMONITOR_SESSION_INFO_MAP_SIZE 1024   // Session Map: 1K (Entries are organized through LRU)

DEFINE_BPF_MAP_GRO(packetmonitor_uid_map, HASH, uint32_t, uint32_t, PACKETMONITOR_UID_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(packetmonitor_pkt_stats_map, HASH, uint32_t, UidPktStats, PACKETMONITOR_UID_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(packetmonitor_port_cnt_map, HASH, uint32_t, int32_t, PACKETMONITOR_UID_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(packetmonitor_dest6_map, LRU_HASH, uint16_t, uint32_t, PACKETMONITOR_DEST6_MAP_SIZE, AID_SYSTEM) 
DEFINE_BPF_MAP_GRO(packetmonitor_uid_port_map, LRU_HASH, MultiKeyForPortCntPerUid, uint16_t, PACKETMONITOR_UID_PORT_MAP_SIZE, AID_SYSTEM)
DEFINE_BPF_MAP_GRO(packetmonitor_session_info_map, LRU_HASH, MultiKeyUidPort, SessionInfo, PACKETMONITOR_SESSION_INFO_MAP_SIZE, AID_SYSTEM)

// Debug logging disabled for production
// #define bpf_debug(fmt, ...)  \
//     ({                       \
//      char ____fmt[] = fmt;\
//      bpf_trace_printk(____fmt, sizeof(____fmt), ##__VA_ARGS__); \
//      })
#define bpf_debug(fmt, ...) ((void)0)

static inline uint32_t packetmonitor_is_uid_allowed(struct __sk_buff *skb) {

    uint32_t sock_uid = bpf_get_socket_uid(skb);
    uint32_t *isMatch = bpf_packetmonitor_uid_map_lookup_elem(&sock_uid);
    if (isMatch && *isMatch == 1) {
        return sock_uid;
    }

    return BPF_NOMATCH;
}

DEFINE_BPF_PROG("schedcls/ingress/packetmonitor", AID_ROOT, AID_SYSTEM, schedcls_ingress_packetmonitor)
(struct __sk_buff *skb) {
    bpf_debug("[Packet monitoring] Ingress entry\n");
 
    // At the TC ingress hook, skb->data points to the network header (IP header).
    // The kernel has already parsed the Ethernet header and stored the protocol
    // type in skb->protocol.
    __u16 eth_proto = skb->protocol;

    uint16_t remote_port_host = 0;
    struct tcphdr tcph_buffer;
    __u32 tcp_offset;
    uint32_t l4_payload_len_approx = 1; // Default for SYN/FIN or non-data ACKs
    uint8_t ip_ver = 0;
    uint32_t remote_ipv4 = 0;
    uint32_t remote_ipv6[4] = {0};

    if (eth_proto == htons(ETH_P_IP)) {
        struct iphdr iph_buffer;
        if (bpf_skb_load_bytes(skb, 0, &iph_buffer, sizeof(iph_buffer)) != 0) {
            return TC_ACT_OK;
        }

        if (iph_buffer.version != 4 || iph_buffer.protocol != IPPROTO_TCP) {
            return TC_ACT_OK;
        }

        __u32 iph_ihl = iph_buffer.ihl;
        if (iph_ihl < 5 || iph_ihl > 15) {
            return TC_ACT_OK;
        }

        // The TCP header starts after the IP header.
        // IP header length is given by iph_ihl * 4.
        tcp_offset = iph_ihl * 4;
        if (bpf_skb_load_bytes(skb, tcp_offset, &tcph_buffer, sizeof(tcph_buffer)) != 0) {
            return TC_ACT_OK;
        }

        remote_port_host = ntohs(tcph_buffer.dest);
        ip_ver = 4;
        remote_ipv4 = iph_buffer.saddr;

        if (!(tcph_buffer.syn || tcph_buffer.fin)) {
            l4_payload_len_approx = ntohs(iph_buffer.tot_len) - (iph_buffer.ihl * 4) - (tcph_buffer.doff * 4);
            if ((int32_t)l4_payload_len_approx < 0) l4_payload_len_approx = 0;
        }
    } else if (eth_proto == htons(ETH_P_IPV6)) {
        struct ipv6hdr ip6h_buffer;
        if (bpf_skb_load_bytes(skb, 0, &ip6h_buffer, sizeof(ip6h_buffer)) != 0) {
            return TC_ACT_OK;
        }

        if (ip6h_buffer.nexthdr != IPPROTO_TCP) {
            return TC_ACT_OK;
        }

        // The TCP header starts after the fixed-size IPv6 header.
        tcp_offset = sizeof(struct ipv6hdr);
        if (bpf_skb_load_bytes(skb, tcp_offset, &tcph_buffer, sizeof(tcph_buffer)) != 0) {
            return TC_ACT_OK;
        }

        remote_port_host = ntohs(tcph_buffer.dest);
        ip_ver = 6;
        remote_ipv6[0] = ip6h_buffer.saddr.in6_u.u6_addr32[0];
        remote_ipv6[1] = ip6h_buffer.saddr.in6_u.u6_addr32[1];
        remote_ipv6[2] = ip6h_buffer.saddr.in6_u.u6_addr32[2];
        remote_ipv6[3] = ip6h_buffer.saddr.in6_u.u6_addr32[3];
        
        if (!(tcph_buffer.syn || tcph_buffer.fin)) {
            l4_payload_len_approx = ntohs(ip6h_buffer.payload_len) - (tcph_buffer.doff * 4);
            if ((int32_t)l4_payload_len_approx < 0) l4_payload_len_approx = 0;
        }
    } else {
        return TC_ACT_OK; // Not IPv4 or IPv6
    }

    // remote_port_host is guaranteed to be non-zero if TCP header parsing succeeded

    // UID determination
    uint32_t uuid = 0;
    uint32_t *uid_ptr = bpf_packetmonitor_dest6_map_lookup_elem(&remote_port_host);

    if (uid_ptr) {
        uuid = *uid_ptr;
    } else {
        uint32_t local_socket_uid = bpf_get_socket_uid(skb);
        uint32_t *is_monitored = bpf_packetmonitor_uid_map_lookup_elem(&local_socket_uid);
        if (is_monitored && *is_monitored == 1) {
            uuid = local_socket_uid;
            // If an incoming connection is for a monitored local UID, and not yet in dest6_map,
            // update dest6_map. This helps associate this remote_port with this local_socket_uid.
            if (uuid !=0) bpf_packetmonitor_dest6_map_update_elem(&remote_port_host, &uuid, BPF_ANY);
        } else {
            return TC_ACT_OK; // Not for a known monitored session/UID
        }
    }
    if (uuid == 0) return TC_ACT_OK; // No valid UID found to associate with

    // Update UidPktStats
    uint64_t packet_len = skb->len;
    UidPktStats *stats = bpf_packetmonitor_pkt_stats_map_lookup_elem(&uuid);
    if (stats) {
        __sync_fetch_and_add(&stats->total_bytes, packet_len);
        __sync_fetch_and_add(&stats->total_packets, 1);
    } else {
        UidPktStats new_stats = {packet_len, 1};
        bpf_packetmonitor_pkt_stats_map_update_elem(&uuid, &new_stats, BPF_ANY);
    }

    uint64_t current_ts = bpf_ktime_get_ns();
    uint8_t flags_byte;
    if (bpf_skb_load_bytes(skb, tcp_offset + TCP_FLAG_OFF, &flags_byte, 1) < 0) {
        return TC_ACT_OK;
    }
    uint8_t tcp_flags_val = flags_byte & 0x3F;

    bpf_debug("[Packet monitoring][ingress] time: %" PRIu64 ", tcp_flag: 0x%02x\n", current_ts, tcp_flags_val);

    MultiKeyUidPort session_key = {uuid, remote_port_host}; // SessionInfo map key: (uid, remote_port_host_order)
    SessionInfo *s_info = bpf_packetmonitor_session_info_map_lookup_elem(&session_key);

    if (s_info) {
        SessionInfo update_s_info = *s_info;
        bpf_debug("[Packet monitoring][ingress] s_info already exists");
        
        __sync_fetch_and_add(&update_s_info.total_pkt, 1);
        
        bool syn_flag = tcp_flags_val & TCP_FLAG_SYN;

        // Detected port reuse due to process restart: Only initialize TCP state if it's a SYN packet
        if (syn_flag) {
            bpf_debug("[Packet monitoring][ingress] SYN on existing session - process restart detected, reinitializing TCP state only");
            // Only initialize TCP connection state information and preserve statistics counters
            // Reset only the fields for sequence/ACK tracking for new connections
            // Retain the retransmission/OOO/RST/Dup ACK/zero window counts to maintain accumulated statistics.
            update_s_info.session_start_ts = current_ts;  // Update new session start time
            update_s_info.expected_seq_server = ntohl(tcph_buffer.seq) + 1;
            update_s_info.last_ack_from_client = ntohl(tcph_buffer.ack_seq);
            update_s_info.consecutive_dup_acks_server = (tcp_flags_val & TCP_FLAG_ACK) ? 1 : 0;
            update_s_info.last_ingress_seq_num = ntohl(tcph_buffer.seq);
            update_s_info.last_ingress_ts = current_ts;
            update_s_info.last_pkt_ts = current_ts;
            update_s_info.fin_from_server_seen = 0;  // Initialize FIN state
            if (tcp_flags_val & TCP_FLAG_ACK) {
                update_zero_window(&update_s_info, tcph_buffer.window, false);
            }

            bpf_packetmonitor_session_info_map_update_elem(&session_key, &update_s_info, BPF_EXIST);
            return TC_ACT_OK;
        }
        
        update_s_info.last_pkt_ts = current_ts;

        if (tcp_flags_val & TCP_FLAG_RST) {
            __sync_fetch_and_add(&update_s_info.rst_server, 1);
        } else {
            uint32_t current_seq = ntohl(tcph_buffer.seq);
            if (update_s_info.expected_seq_server > 0) {
                 if (current_seq < update_s_info.expected_seq_server) {
                    __sync_fetch_and_add(&update_s_info.retrans_server, 1);
                } else if (current_seq > update_s_info.expected_seq_server) {
                    __sync_fetch_and_add(&update_s_info.out_of_order_server, 1);
                }
            }
            // Update expected sequence number for non-retransmitted packets
            if (current_seq >= update_s_info.expected_seq_server) {
                update_s_info.expected_seq_server = current_seq;
                if (tcp_flags_val & (TCP_FLAG_SYN | TCP_FLAG_FIN)) {
                    __sync_fetch_and_add(&update_s_info.expected_seq_server, 1);
                } else {
                    __sync_fetch_and_add(&update_s_info.expected_seq_server, l4_payload_len_approx);
                }
            }
        }

        if (tcp_flags_val & TCP_FLAG_FIN) update_s_info.fin_from_server_seen = 1;

        if (tcp_flags_val & TCP_FLAG_ACK) {
            update_zero_window(&update_s_info, tcph_buffer.window, false);
            uint32_t current_ack = ntohl(tcph_buffer.ack_seq);
            if (current_ack == update_s_info.last_ack_from_client) {
                if (update_s_info.consecutive_dup_acks_server < 255) {
                    __sync_fetch_and_add(&update_s_info.consecutive_dup_acks_server, 1);
                }
                if (update_s_info.consecutive_dup_acks_server == 3) {
                     __sync_fetch_and_add(&update_s_info.dup_ack_events_server, 1);
                }
            } else {
                update_s_info.last_ack_from_client = current_ack;
                update_s_info.consecutive_dup_acks_server = 1;
            }
        }

        // RTT calculation: Check if this ACK acknowledges previously sent data packets
        if (tcp_flags_val & TCP_FLAG_ACK) {
            uint32_t current_ack = ntohl(tcph_buffer.ack_seq);
            
            // Iterating through the in-flight buffer to find ACK packets
            #pragma unroll
            for (int i = 0; i < MAX_INFLIGHT_PACKETS; i++) {
                // Check if there is a valid timestamp and whether it is a packet confirmed by the current ACK
                if (update_s_info.inflight[i].ts > 0 && current_ack > update_s_info.inflight[i].seq) {
                    uint64_t rtt_ns = current_ts - update_s_info.inflight[i].ts;

                    // RTT must be greater than 0 and less than 15 seconds
                    if (rtt_ns > 0 && rtt_ns < 15000000000ULL) {
                        __sync_fetch_and_add(&update_s_info.rtt_total_ns, rtt_ns);
                        __sync_fetch_and_add(&update_s_info.rtt_count, 1);
                    }

                    // Items with completed calculations are invalidated by setting the timestamp to 0 (to prevent duplicate calculations)
                    update_s_info.inflight[i].ts = 0;
                }
            }
        }

        update_s_info.last_ingress_seq_num = ntohl(tcph_buffer.seq);
        update_s_info.last_ingress_ts = current_ts;

		bpf_packetmonitor_session_info_map_update_elem(&session_key, &update_s_info, BPF_EXIST);
    } else if (tcp_flags_val & TCP_FLAG_SYN) {
        SessionInfo new_s_info = {0};
        new_s_info.session_start_ts = current_ts;
        new_s_info.last_pkt_ts = current_ts;
        new_s_info.port_key_for_seqs = remote_port_host;
        new_s_info.expected_seq_server = ntohl(tcph_buffer.seq) + 1;
        new_s_info.last_ack_from_client = ntohl(tcph_buffer.ack_seq);
        new_s_info.consecutive_dup_acks_server = (tcp_flags_val & TCP_FLAG_ACK) ? 1 : 0;
        new_s_info.last_ingress_seq_num = ntohl(tcph_buffer.seq);
        new_s_info.last_ingress_ts = current_ts;
        new_s_info.inflight_idx = 0;
        new_s_info.total_pkt = 1;
        new_s_info.ip_version = ip_ver;

        if (ip_ver == 4) {
            new_s_info.remote_ip[0] = remote_ipv4;
            new_s_info.remote_ip[1] = 0;
            new_s_info.remote_ip[2] = 0;
            new_s_info.remote_ip[3] = 0;
        } else if (ip_ver == 6) {
            new_s_info.remote_ip[0] = remote_ipv6[0];
            new_s_info.remote_ip[1] = remote_ipv6[1];
            new_s_info.remote_ip[2] = remote_ipv6[2];
            new_s_info.remote_ip[3] = remote_ipv6[3];
        }
        
        if (tcp_flags_val & TCP_FLAG_ACK) {
            update_zero_window(&new_s_info, tcph_buffer.window, false);
        }

        if (bpf_packetmonitor_session_info_map_update_elem(&session_key, &new_s_info, BPF_NOEXIST) == 0) {
            int32_t *port_count_ptr = bpf_packetmonitor_port_cnt_map_lookup_elem(&uuid);
            int32_t current_port_count = port_count_ptr ? *port_count_ptr : 0;
            current_port_count++;
            bpf_packetmonitor_port_cnt_map_update_elem(&uuid, &current_port_count, BPF_ANY);
            MultiKeyForPortCntPerUid uid_port_map_key = {uuid, current_port_count};
            bpf_packetmonitor_uid_port_map_update_elem(&uid_port_map_key, &remote_port_host, BPF_ANY);
            bpf_packetmonitor_dest6_map_update_elem(&remote_port_host, &uuid, BPF_ANY);
        }
    } 

    return TC_ACT_OK;
}

DEFINE_BPF_PROG("schedcls/egress/packetmonitor", AID_ROOT, AID_SYSTEM, schedcls_egress_packetmonitor)
(struct __sk_buff *skb) {
    bpf_debug("[Packet monitoring] Egress entry\n");

    uint32_t allowedUID = packetmonitor_is_uid_allowed(skb);
    if (allowedUID == BPF_NOMATCH) {
        return TC_ACT_OK;
    }

    // At the TC egress hook, skb->data points to the network header (IP header).
    // The kernel has already parsed the Ethernet header and stored the protocol
    // type in skb->protocol.
    __u16 eth_proto = skb->protocol;

    struct tcphdr tcph_buffer;
    __u32 tcp_offset;
    uint16_t local_port_host = 0, remote_port_host = 0;
    uint32_t l4_payload_len_approx = 1; // Default for SYN/FIN or non-data ACKs
    uint8_t ip_ver = 0;
    uint32_t remote_ipv4 = 0;
    uint32_t remote_ipv6[4] = {0};

    if (eth_proto == htons(ETH_P_IP)) {
        struct iphdr iph_buffer;
        if (bpf_skb_load_bytes(skb, 0, &iph_buffer, sizeof(iph_buffer)) != 0) {
            return TC_ACT_OK;
        }
        if (iph_buffer.version != 4 || iph_buffer.protocol != IPPROTO_TCP) {
            return TC_ACT_OK;
        }

        __u32 iph_ihl = iph_buffer.ihl;
        if (iph_ihl < 5 || iph_ihl > 15) {
            return TC_ACT_OK;
        }

        tcp_offset = iph_ihl * 4;
        if (bpf_skb_load_bytes(skb, tcp_offset, &tcph_buffer, sizeof(tcph_buffer)) != 0) {
            return TC_ACT_OK;
        }

        local_port_host = ntohs(tcph_buffer.source);
        remote_port_host = ntohs(tcph_buffer.dest);
        ip_ver = 4;
        remote_ipv4 = iph_buffer.daddr;
        
        if (!(tcph_buffer.syn || tcph_buffer.fin)) {
            l4_payload_len_approx = ntohs(iph_buffer.tot_len) - (iph_buffer.ihl * 4) - (tcph_buffer.doff * 4);
            if ((int32_t)l4_payload_len_approx < 0) l4_payload_len_approx = 0;
        }
    } else if (eth_proto == htons(ETH_P_IPV6)) {
        struct ipv6hdr ip6h_buffer;
        if (bpf_skb_load_bytes(skb, 0, &ip6h_buffer, sizeof(ip6h_buffer)) != 0) {
            return TC_ACT_OK;
        }
        if (ip6h_buffer.nexthdr != IPPROTO_TCP) {
            return TC_ACT_OK;
        }

        tcp_offset = sizeof(struct ipv6hdr);
        if (bpf_skb_load_bytes(skb, tcp_offset, &tcph_buffer, sizeof(tcph_buffer)) != 0) {
            return TC_ACT_OK;
        }

        local_port_host = ntohs(tcph_buffer.source);
        remote_port_host = ntohs(tcph_buffer.dest);
        ip_ver = 6;
        remote_ipv6[0] = ip6h_buffer.daddr.in6_u.u6_addr32[0];
        remote_ipv6[1] = ip6h_buffer.daddr.in6_u.u6_addr32[1];
        remote_ipv6[2] = ip6h_buffer.daddr.in6_u.u6_addr32[2];
        remote_ipv6[3] = ip6h_buffer.daddr.in6_u.u6_addr32[3];
        
        if (!(tcph_buffer.syn || tcph_buffer.fin)) {
            l4_payload_len_approx = ntohs(ip6h_buffer.payload_len) - (tcph_buffer.doff * 4);
            if ((int32_t)l4_payload_len_approx < 0) l4_payload_len_approx = 0;
        }
    } else {
        return TC_ACT_OK; // Not IPv4 or IPv6
    }

    if (local_port_host == 0 || remote_port_host == 0) {
        return TC_ACT_OK; 
    }

    // Update UidPktStats for the allowedUID
    uint64_t packet_len = skb->len;
    UidPktStats *stats = bpf_packetmonitor_pkt_stats_map_lookup_elem(&allowedUID);
    if (stats) {
        __sync_fetch_and_add(&stats->total_bytes, packet_len);
        __sync_fetch_and_add(&stats->total_packets, 1);
    } else {
        UidPktStats new_stats = {packet_len, 1};
        bpf_packetmonitor_pkt_stats_map_update_elem(&allowedUID, &new_stats, BPF_ANY);
    }

    uint64_t current_ts = bpf_ktime_get_ns();
    uint8_t flags_byte;
    if (bpf_skb_load_bytes(skb, tcp_offset + TCP_FLAG_OFF, &flags_byte, 1) < 0) {
        return TC_ACT_OK;
    }
    uint8_t tcp_flags_val = flags_byte & 0x3F;

    bpf_debug("[Packet monitoring][egress] time: %" PRIu64 ", tcp_flag: 0x%02x\n", current_ts, tcp_flags_val);

    MultiKeyUidPort session_key = {allowedUID, local_port_host};
    SessionInfo *s_info = bpf_packetmonitor_session_info_map_lookup_elem(&session_key);

    if (s_info) {
        SessionInfo update_s_info = *s_info;
        bpf_debug("[Packet monitoring][egress] s_info already exists");
        
        __sync_fetch_and_add(&update_s_info.total_pkt, 1);
        
        bool syn_flag = tcp_flags_val & TCP_FLAG_SYN;

        // Detected port reuse due to process restart: Only initialize TCP state if it's a SYN packet
        if (syn_flag) {
            bpf_debug("[Packet monitoring][egress] SYN on existing session - process restart detected, reinitializing TCP state only");
            // Only initialize TCP connection state information and preserve statistics counters
            // Reset only the fields for sequence/ACK tracking for new connections
            // Retain the retransmission/OOO/RST/Dup ACK/zero window counts to maintain accumulated statistics.
            update_s_info.session_start_ts = current_ts;  // Update new session start time
            update_s_info.expected_seq_client = ntohl(tcph_buffer.seq) + 1;
            if (tcp_flags_val & TCP_FLAG_ACK) {
                update_s_info.last_ack_from_server = ntohl(tcph_buffer.ack_seq);
                update_s_info.consecutive_dup_acks_client = 1;
                update_zero_window(&update_s_info, tcph_buffer.window, true);
            }
            update_s_info.last_egress_seq_num = ntohl(tcph_buffer.seq);
            update_s_info.last_egress_ts = current_ts;
            update_s_info.last_pkt_ts = current_ts;
            update_s_info.fin_from_client_seen = 0;  // Initialize FIN state

            bpf_packetmonitor_session_info_map_update_elem(&session_key, &update_s_info, BPF_EXIST);
            return TC_ACT_OK;
        }
        
        update_s_info.last_pkt_ts = current_ts;

        if (tcp_flags_val & TCP_FLAG_RST) {
            __sync_fetch_and_add(&update_s_info.rst_client, 1);
        } else {
            uint32_t current_seq = ntohl(tcph_buffer.seq);
            if (update_s_info.expected_seq_client > 0) {
                if (current_seq < update_s_info.expected_seq_client) {
                    __sync_fetch_and_add(&update_s_info.retrans_client, 1);
                } else if (current_seq > update_s_info.expected_seq_client) {
                    __sync_fetch_and_add(&update_s_info.out_of_order_client, 1);
                }
            }

            // Update expected sequence number for non-retransmitted packets
            if (current_seq >= update_s_info.expected_seq_client) {
                update_s_info.expected_seq_client = current_seq;
                if (tcp_flags_val & (TCP_FLAG_SYN | TCP_FLAG_FIN)) {
                   __sync_fetch_and_add(&update_s_info.expected_seq_client, 1);
                } else {
                   __sync_fetch_and_add(&update_s_info.expected_seq_client, l4_payload_len_approx);
                }
            }
        }
        if (tcp_flags_val & TCP_FLAG_FIN) update_s_info.fin_from_client_seen = 1;

        if (tcp_flags_val & TCP_FLAG_ACK) { // ACK field in our outgoing packet
            update_zero_window(&update_s_info, tcph_buffer.window, true);
            uint32_t current_ack = ntohl(tcph_buffer.ack_seq);
            if (current_ack == update_s_info.last_ack_from_server) {
                if (update_s_info.consecutive_dup_acks_client < 255) {
                    __sync_fetch_and_add(&update_s_info.consecutive_dup_acks_client, 1);
                }
                if (update_s_info.consecutive_dup_acks_client == 3) {
                     __sync_fetch_and_add(&update_s_info.dup_ack_events_client, 1);
                }
            } else {
                update_s_info.last_ack_from_server = current_ack;
                update_s_info.consecutive_dup_acks_client = 1;
            }
        }
        // RTT tracking: Record timestamp and sequence number for data/SYN/FIN packets
        if ((l4_payload_len_approx > 1) || (tcp_flags_val & (TCP_FLAG_SYN | TCP_FLAG_FIN))) {
            // Save the sequence number and timestamp at the current index
            uint8_t current_idx = update_s_info.inflight_idx;
            if (current_idx < MAX_INFLIGHT_PACKETS) {
                update_s_info.inflight[current_idx].seq = ntohl(tcph_buffer.seq);
                update_s_info.inflight[current_idx].ts = current_ts;

                // Cycling through the index for the next save
                update_s_info.inflight_idx = (current_idx + 1) % MAX_INFLIGHT_PACKETS;
            }
        }

        update_s_info.last_egress_seq_num = ntohl(tcph_buffer.seq);
        update_s_info.last_egress_ts = current_ts;
        bpf_packetmonitor_session_info_map_update_elem(&session_key, &update_s_info, BPF_EXIST);
    } else if (tcp_flags_val & TCP_FLAG_SYN) { // New session initiated by us
        SessionInfo new_s_info = {0};
        new_s_info.session_start_ts = current_ts;
        new_s_info.last_pkt_ts = current_ts;
        new_s_info.port_key_for_seqs = remote_port_host;
        new_s_info.expected_seq_client = ntohl(tcph_buffer.seq) + 1;
        // new_s_info.expected_seq_server = 0; // Unknown until first ingress
        if (tcp_flags_val & TCP_FLAG_ACK) {
            new_s_info.last_ack_from_server = ntohl(tcph_buffer.ack_seq);
            new_s_info.consecutive_dup_acks_client = 1;
            update_zero_window(&new_s_info, tcph_buffer.window, true);
        }
        new_s_info.last_egress_seq_num = ntohl(tcph_buffer.seq);
        new_s_info.last_egress_ts = current_ts;
        new_s_info.inflight_idx = 0;
        new_s_info.total_pkt = 1;
        new_s_info.ip_version = ip_ver;

        if (ip_ver == 4) {
            new_s_info.remote_ip[0] = remote_ipv4;
            new_s_info.remote_ip[1] = 0;
            new_s_info.remote_ip[2] = 0;
            new_s_info.remote_ip[3] = 0;
        } else if (ip_ver == 6) {
            new_s_info.remote_ip[0] = remote_ipv6[0];
            new_s_info.remote_ip[1] = remote_ipv6[1];
            new_s_info.remote_ip[2] = remote_ipv6[2];
            new_s_info.remote_ip[3] = remote_ipv6[3];
        }

        if (bpf_packetmonitor_session_info_map_update_elem(&session_key, &new_s_info, BPF_NOEXIST) == 0) {
            int32_t *port_count_ptr = bpf_packetmonitor_port_cnt_map_lookup_elem(&allowedUID);
            int32_t current_port_count = port_count_ptr ? *port_count_ptr : 0;
            current_port_count++;

            bpf_packetmonitor_port_cnt_map_update_elem(&allowedUID, &current_port_count, BPF_ANY);
            MultiKeyForPortCntPerUid uid_port_map_key = {allowedUID, current_port_count};
            bpf_packetmonitor_uid_port_map_update_elem(&uid_port_map_key, &local_port_host, BPF_ANY);
            bpf_packetmonitor_dest6_map_update_elem(&local_port_host, &allowedUID, BPF_ANY);
        }
    }

    return TC_ACT_OK;
}
LICENSE("GPL");
