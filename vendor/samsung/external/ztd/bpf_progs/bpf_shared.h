
#ifndef BPF_SHARED_H
#define BPF_SHARED_H

#include <stdbool.h>
#include <stdint.h>

#include <android_bpf_defs.h>

#define POSSIBLE_CPU_CNT 8

#define DEFINE_BPF_PERCPU_EVENT_CACHE(the_map, KeyType, ValueType, percpu_event_entries_num, gid) \
    _Static_assert((percpu_event_entries_num) > 1, "percpu event entries too small in size");     \
                                                                                                  \
    DEFINE_BPF_MAP_BASE(the_map, HASH, KeyType, ValueType,                                        \
                        (percpu_event_entries_num * POSSIBLE_CPU_CNT), gid, BPF_F_NO_PREALLOC);   \
    static inline __always_inline __unused uint64_t circ_percpu_cache(const uint64_t cpu_id,      \
                                                                      const uint64_t event_cnt) { \
        return ((((cpu_id) % POSSIBLE_CPU_CNT) * percpu_event_entries_num) |                      \
                ((event_cnt) & (percpu_event_entries_num - 1)));                                  \
    }

#define DEFINE_BPF_MAP_HASH_NPA(the_map, KeyType, ValueType, num_entries, gid) \
    DEFINE_BPF_MAP_BASE(the_map, HASH, KeyType, ValueType, num_entries, gid, BPF_F_NO_PREALLOC)

#define DEFINE_ZT_TRACEPOINT(the_system, the_event) \
    DEFINE_BPF_PROG("tracepoint/" #the_system "/" #the_event, AID_ROOT, AID_SYSTEM, tp_##the_event)

#define DEFINE_ZT_KPROBE(the_ksymbol) \
    DEFINE_BPF_PROG("kprobe/" #the_ksymbol, AID_ROOT, AID_SYSTEM, kp##the_ksymbol)

/*
 * POINTER_OF_USER_SPACE must be used only by 'bpf_probe_read_user/str'.
 * It does not work for 'bpf_probe_read_kernel'
 */
#define POINTER_OF_USER_SPACE(reg) ((void*)(reg & 0x0000FFFFFFFFFFFF))

#if !defined(RETURN_ALLOW)
#define RETURN_ALLOW return 1     // return 1 to avoid blocking simpleperf from receiving events
#endif

#define ENSURE_TASK                     struct task_struct* task =                              \
                                            (struct task_struct *) bpf_get_current_task();      \
                                        if (!task) {                                            \
                                            RETURN_ALLOW;                                       \
                                        }

#define ENSURE_CTX                      kzt_context_t* ctx;                                     \
                                        {                                                       \
                                            uint32_t zero = 0;                                  \
                                            ctx = bpf_context_map_lookup_elem(&zero);           \
                                            if (!ctx) {                                         \
                                                RETURN_ALLOW;                                   \
                                            }                                                   \
                                        }

typedef struct {
    uint32_t value:12;
} uint12_t;

#endif // BPF_SHARED_H
