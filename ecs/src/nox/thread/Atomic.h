#ifndef NOX_THREAD_ATOMIC_H_
#define NOX_THREAD_ATOMIC_H_
#include <atomic>

#ifdef NOX_ATOMIC_USE_SEQ_CST
    #define NOX_ATOMIC_RELAXED std::memory_order_seq_cst
    #define NOX_ATOMIC_CONSUME std::memory_order_seq_cst
    #define NOX_ATOMIC_ACQUIRE std::memory_order_seq_cst
    #define NOX_ATOMIC_RELEASE std::memory_order_seq_cst
    #define NOX_ATOMIC_ACQ_REL std::memory_order_seq_cst
    #define NOX_ATOMIC_SEQ_CST std::memory_order_seq_cst 
#else
    #define NOX_ATOMIC_RELAXED std::memory_order_relaxed
    #define NOX_ATOMIC_CONSUME std::memory_order_consume
    #define NOX_ATOMIC_ACQUIRE std::memory_order_acquire
    #define NOX_ATOMIC_RELEASE std::memory_order_release
    #define NOX_ATOMIC_ACQ_REL std::memory_order_acq_rel
    #define NOX_ATOMIC_SEQ_CST std::memory_order_seq_cst
#endif

#endif
