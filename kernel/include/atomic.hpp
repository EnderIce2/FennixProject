#pragma once

#if !__has_extension(c_atomic)
#define _Atomic(T) T
#endif

#define builtin_atomic(name) __atomic_##name##_n

namespace
{
    enum MemoryBorder
    {
        Relaxed = __ATOMIC_RELAXED,
        Acquire = __ATOMIC_ACQUIRE,
        Release = __ATOMIC_RELEASE,
        AcqRel = __ATOMIC_ACQ_REL,
        SeqCst = __ATOMIC_SEQ_CST
    };
    template <typename T>
    class atomic
    {
        _Atomic(T) value;

    public:
        atomic(T Init) : value(Init) {}
        T load(MemoryBorder order = MemoryBorder::SeqCst)
        {
            return builtin_atomic(load)(&value, order);
        }
        void store(T v, MemoryBorder order = MemoryBorder::SeqCst)
        {
            return builtin_atomic(store)(&value, v, order);
        }
        T exchange(T v, MemoryBorder order = MemoryBorder::SeqCst)
        {
            return builtin_atomic(exchange)(&value, v, order);
        }
        bool compare_exchange(T &expected, T desired, MemoryBorder order = MemoryBorder::SeqCst)
        {
            return __atomic_compare_exchange_n(&value, &expected, desired, true, order, order);
        }
    };
}
#undef builtin_atomic
