/******************************************************************************************************
 *  FILENAME: tcpAccECN.c
 *
 *  DESCRIPTION :
 *        eBPF based implementation of AccECN for IPv4 TCP connections.
 *
 *  AUTHOR : Jayendra Reddy Kovvuri, Madhan Raj Kanagarathinam
 *  DATE: 2025
 *  VERSION: 1.2
 *
 *  NOTE:
 *  1.1: Receiver side L4S (AccECN) functionality using eBPF.
 *  1.2: enabling L4S on a per-UID basis.
 *
 *  COPYRIGHT BY Samsung Electronics. ALL RIGHTS RESERVED
 ******************************************************************************************************/
#include <linux/bpf.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <stdint.h>
#include <stdbool.h>
#include <android_bpf_defs.h>
#include <ss_bpf_shared.h>
#include <linux/pkt_cls.h>
#include <linux/filter.h>

// Offsets from beginning of L4 (TCP) header
#define TCP_OFFSET(field) offsetof(struct tcphdr, field)

// Offsets from beginning of L3 (IPv4) header
#define IP4_OFFSET(field) offsetof(struct iphdr, field)
#define IP4_TCP_OFFSET(field) (sizeof(struct iphdr) + TCP_OFFSET(field))

// Offsets from beginning of L2 (ie. Ethernet) header (which must be present)
#define ETH_IP4_OFFSET(field) (ETH_HLEN + IP4_OFFSET(field))
#define ETH_IP4_TCP_OFFSET(field) (ETH_HLEN + IP4_TCP_OFFSET(field))

#define TCP_FLAGS_OFF (ETH_HLEN + sizeof(struct iphdr) + 12)

// an LRU map discards the least recently used entry when it is full.
#define L4S_ACCECN_MAP_SIZE 128
DEFINE_BPF_MAP_GRW(l4s_accecn_ce_map, LRU_HASH, uint32_t, uint32_t, L4S_ACCECN_MAP_SIZE, AID_SYSTEM)

#define SEMAPE_RT_UID_MAP_SIZE 1024
DEFINE_BPF_MAP_GRW(l4s_rt_uid_map, LRU_HASH, uint32_t, uint8_t, SEMAPE_RT_UID_MAP_SIZE, AID_SYSTEM)

#define BPF_NOMATCH 0
#define BPF_MATCH 1

#define ALL_UID_ON 0xFFFFFFFF

// Static const declarations for BTF compatibility
static const uint32_t INIT_VALUE_CE_MARKED = 0b110;
static const uint32_t INIT_VALUE_NOT_CE_MARKED = 0b101;
static const uint16_t ACE_VALID_010 = 0b010;
static const uint16_t ACE_VALID_011 = 0b011;
static const uint16_t ACE_VALID_100 = 0b100;
static const uint16_t ACE_VALID_110 = 0b110;
static const uint16_t ACE_REQUESTING_ACCECN = 0b111;
static const uint8_t ECN_CE_MARKED = 0b11;

// Android only supports little endian architectures
#define htons(x) (__builtin_constant_p(x) ? ___constant_swab16(x) : __builtin_bswap16(x))
#define htonl(x) (__builtin_constant_p(x) ? ___constant_swab32(x) : __builtin_bswap32(x))
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)

static __always_inline int is_system_uid(uint32_t uid) {
    // MIN_SYSTEM_UID is AID_ROOT == 0, so uint32_t is *always* >= 0
    // MAX_SYSTEM_UID is AID_NOBODY == 9999, while AID_APP_START == 10000
    return (uid < 10000);
}

static inline bool should_enable_l4s(struct __sk_buff* skb) {
    uint32_t key = ALL_UID_ON;
    uint8_t *enable_l4s_for_all = bpf_l4s_rt_uid_map_lookup_elem(&key);
    if (enable_l4s_for_all) {
        return BPF_MATCH;
    }

    uint32_t sock_uid = bpf_get_socket_uid(skb);
    if (is_system_uid(sock_uid)) return BPF_MATCH;

    uint8_t *rtUidPresent = bpf_l4s_rt_uid_map_lookup_elem(&sock_uid);
    if (rtUidPresent)
        return BPF_MATCH;
    return BPF_NOMATCH;
}

DEFINE_BPF_PROG("schedcls/ingress/l4s_accecn", AID_ROOT, AID_SYSTEM, schedcls_ingress_l4s_accecn)
(struct __sk_buff* skb) {
    void* data = (void*)(long)skb->data;
    void* data_end = (void*)(long)skb->data_end;

    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end) {
        return TC_ACT_PIPE;
    }

    if (skb->protocol == htons(ETH_P_IP)) {
        struct iphdr* ip = data + ETH_HLEN;
        if (ip->protocol == IPPROTO_TCP) {
            if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr) > data_end) {
                return TC_ACT_PIPE;
            }
            struct tcphdr* tcph = (void*)(ip + 1);
            uint32_t key = (tcph->source << 16) | tcph->dest;
            uint32_t *ce_count = bpf_l4s_accecn_ce_map_lookup_elem(&key);
            
            if (!ce_count) {
                // SYN/ACK
                if (tcph->syn && tcph->ack) {
                    __u16 flags = load_half(skb, TCP_FLAGS_OFF);
                    __u16 ace = (flags & 0x01c0) >> 6;

                    // if the ACE is valid, add the entry to the map
                    if (ace == ACE_VALID_010 || ace == ACE_VALID_011 || ace == ACE_VALID_100 || ace == ACE_VALID_110) {
                        uint32_t init_value = ((ip->tos & 0x03) == ECN_CE_MARKED) ? INIT_VALUE_CE_MARKED : INIT_VALUE_NOT_CE_MARKED;
                        bpf_l4s_accecn_ce_map_update_elem(&key, &init_value, 0);
                        
                        uint32_t conn_key = 0;
                        uint32_t *conn_count = bpf_l4s_accecn_ce_map_lookup_elem(&conn_key);
                        uint32_t oneConnection = 1;
                        if (!conn_count) {
                            bpf_l4s_accecn_ce_map_update_elem(&conn_key, &oneConnection, 0);
                        } else {
                            __sync_fetch_and_add(conn_count, oneConnection);
                        }
                        return TC_ACT_OK;
                    }
                    return TC_ACT_PIPE;
                }
            } else {
                // if FIN or RST, remove entry from the map
                if (tcph->fin || tcph->rst) {
                    bpf_l4s_accecn_ce_map_delete_elem(&key);
                    return TC_ACT_OK;
                }

                // update the map if CE is marked
                if ((ip->tos & 0x03) == ECN_CE_MARKED) {
                    uint32_t onePacket = 1;
                    __sync_fetch_and_add(ce_count, onePacket);
                    return TC_ACT_OK;
                }
            }
        }
    }
    return TC_ACT_PIPE;
}

DEFINE_BPF_PROG("schedcls/egress/l4s_accecn", AID_ROOT, AID_SYSTEM, schedcls_egress_l4s_accecn)
(struct __sk_buff* skb) {
    void* data = (void*)(long)skb->data;
    void* data_end = (void*)(long)skb->data_end;

    if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) > data_end) {
        return TC_ACT_PIPE;
    }

    if (skb->protocol == htons(ETH_P_IP)) {
        struct iphdr* ip = data + ETH_HLEN;
        if (ip->protocol == IPPROTO_TCP) {
            if (data + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr) > data_end) {
                return TC_ACT_PIPE;
            }
            struct tcphdr* tcph = (void*)(ip + 1);
            uint32_t key = (tcph->dest << 16) | tcph->source;
            bool l4s_supported =  should_enable_l4s(skb);
            if(l4s_supported) {
                // if SYN, then set ACE to 111
                if (tcph->syn && !tcph->ack) {
                    __u16 cur_flags = load_half(skb, TCP_FLAGS_OFF);
                    __u16 new_flags = htons(cur_flags | 0x01c0);
                    __u16 cur_ace = (cur_flags & 0x01c0) >> 6;

                    // connection requesting AccECN by default
                    if (cur_ace == ACE_REQUESTING_ACCECN) {
                        return TC_ACT_OK;
                    } else {
                        int ret1 = bpf_l4_csum_replace(skb, ETH_IP4_TCP_OFFSET(check), htons(cur_flags), new_flags, 2);
                        int ret2 = bpf_skb_store_bytes(skb, TCP_FLAGS_OFF, &new_flags, sizeof(new_flags), 0);
                        if (ret1 || ret2) {
                            return TC_ACT_PIPE;
                        }
                    }
                    return TC_ACT_OK;
                }

                // if present in map set ACE value and IP ECN bits
                uint32_t *ce_count = bpf_l4s_accecn_ce_map_lookup_elem(&key);
                if (ce_count) {
                    __u16 cur_flags = load_half(skb, TCP_FLAGS_OFF);
                    __u16 new_flags = htons((cur_flags & 0xfe3f) | ((*ce_count & 7) << 6)) ;
                    
                    bpf_l4_csum_replace(skb, ETH_IP4_TCP_OFFSET(check), htons(cur_flags), new_flags, 2);
                    bpf_skb_store_bytes(skb, TCP_FLAGS_OFF, &new_flags, sizeof(new_flags), 0);
                    
                    __u8 old_tos = load_byte(skb, ETH_IP4_OFFSET(tos));
                    __u8 new_tos = old_tos | 0x01;
                    
                    bpf_l3_csum_replace(skb, ETH_IP4_OFFSET(check), htons(old_tos), htons(new_tos), 2);
                    bpf_skb_store_bytes(skb, ETH_IP4_OFFSET(tos), &new_tos, sizeof(new_tos), 0);
                    return TC_ACT_OK;
                }
            }
        }
    }
    return TC_ACT_PIPE;
}
LICENSE("GPL");
CRITICAL("Sem eBPF tcpAccECN");
