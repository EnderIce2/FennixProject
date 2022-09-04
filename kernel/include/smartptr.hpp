#pragma once

#include <debug.h>

// show debug messages
// #define DEBUG_SMARTPOINTERS 1

#ifdef DEBUG_SMARTPOINTERS
#define spdbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define spdbg(m, ...)
#endif

/**
 * @brief A smart pointer class
 *
 * This class is a smart pointer class. It is used to manage the lifetime of
 * objects. It is a reference counted pointer, so when the last reference to
 * the object is removed, the object is deleted.
 *
 * Basic Usage:
 * smart_ptr<char> pointer(new char());
 * *pointer = 'a';
 * printf("%c", *pointer); // Prints "a"
 */
template <class T>
class smart_ptr
{
    T *RealPointer;

public:
    explicit smart_ptr(T *p = nullptr)
    {
        spdbg("Smart pointer created (%#lx)", RealPointer);
        RealPointer = p;
    }

    ~smart_ptr()
    {
        spdbg("Smart pointer deleted (%#lx)", RealPointer);
        delete (RealPointer);
    }

    T &operator*()
    {
        spdbg("Smart pointer dereferenced (%#lx)", RealPointer);
        return *RealPointer;
    }

    T *operator->()
    {
        spdbg("Smart pointer dereferenced (%#lx)", RealPointer);
        return RealPointer;
    }
};

template <class T>
class auto_ptr
{
};

template <class T>
class unique_ptr
{
};

template <typename T>
class shared_ptr
{
private:
    class Counter
    {
    private:
        unsigned int RefCount{};

    public:
        Counter() : RefCount(0){};
        Counter(const Counter &) = delete;
        Counter &operator=(const Counter &) = delete;
        ~Counter() {}
        void Reset() { RefCount = 0; }
        unsigned int Get() { return RefCount; }
        void operator++() { RefCount++; }
        void operator++(int) { RefCount++; }
        void operator--() { RefCount--; }
        void operator--(int) { RefCount--; }
    };

    Counter *ReferenceCounter;
    T *RealPointer;

public:
    explicit shared_ptr(T *Pointer = nullptr)
    {
        spdbg("Shared pointer created (%#lx)", RealPointer);
        RealPointer = Pointer;
        ReferenceCounter = new Counter();
        if (Pointer)
            (*ReferenceCounter)++;
    }

    shared_ptr(shared_ptr<T> &SPtr)
    {
        spdbg("Shared pointer copied (%#lx)", RealPointer);
        RealPointer = SPtr.RealPointer;
        ReferenceCounter = SPtr.ReferenceCounter;
        (*ReferenceCounter)++;
    }

    ~shared_ptr()
    {
        spdbg("Shared pointer deleted (%#lx)", RealPointer);
        (*ReferenceCounter)--;
        if (ReferenceCounter->Get() == 0)
        {
            delete ReferenceCounter;
            delete RealPointer;
        }
    }

    unsigned int GetCount()
    {
        spdbg("Shared pointer count (%#lx)", RealPointer);
        return ReferenceCounter->Get();
    }

    T *Get()
    {
        spdbg("Shared pointer get (%#lx)", RealPointer);
        return RealPointer;
    }

    T &operator*()
    {
        spdbg("Shared pointer dereference (%#lx)", RealPointer);
        return *RealPointer;
    }

    T *operator->()
    {
        spdbg("Shared pointer dereference (%#lx)", RealPointer);
        return RealPointer;
    }
};

template <class T>
class weak_ptr
{
};
