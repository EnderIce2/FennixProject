#pragma once
#include <stdint.h>
#include <heap.h>
#include <debug.h>

template <class T>
class Vector
{
private:
    uint64_t VectorSize = 0;
    uint64_t VectorCapacity = 0;
    T *VectorBuffer = nullptr;

public:
    typedef T *iterator;

    Vector()
    {
        VectorCapacity = 0;
        VectorSize = 0;
        VectorBuffer = 0;
    }

    Vector(uint64_t Size)
    {
        VectorCapacity = Size;
        VectorSize = Size;
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: Vector( %ld )", Size);
#endif
        VectorBuffer = new T[Size];
    }

    Vector(uint64_t Size, const T &Initial)
    {
        VectorSize = Size;
        VectorCapacity = Size;
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: Vector( %ld %llx )", Size, Initial);
#endif
        VectorBuffer = new T[Size];
        for (uint64_t i = 0; i < Size; i++)
            VectorBuffer[i] = Initial;
    }

    Vector(const Vector<T> &Vector)
    {
        VectorSize = Vector.VectorSize;
        VectorCapacity = Vector.VectorCapacity;
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: Vector( <vector> )->Size: %ld", VectorSize);
#endif
        VectorBuffer = new T[VectorSize];
        for (uint64_t i = 0; i < VectorSize; i++)
            VectorBuffer[i] = Vector.VectorBuffer[i];
    }

    ~Vector()
    {
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: ~Vector( ~%lx )", VectorBuffer);
#endif
        delete[] VectorBuffer;
    }

    void remove(uint64_t Position)
    {
        if (Position >= VectorSize)
            return;
        memset(&*(VectorBuffer + Position), 0, sizeof(T));
        for (uint64_t i = 0; i < VectorSize - 1; i++)
        {
            *(VectorBuffer + Position + i) = *(VectorBuffer + Position + i + 1);
        }
        VectorSize--;
    }

    uint64_t capacity() const { return VectorCapacity; }

    uint64_t size() const { return VectorSize; }

    bool empty() const;

    iterator begin() { return VectorBuffer; }

    iterator end() { return VectorBuffer + size(); }

    T &front() { return VectorBuffer[0]; }

    T &back() { return VectorBuffer[VectorSize - 1]; }

    void push_back(const T &Value)
    {
        if (VectorSize >= VectorCapacity)
            reserve(VectorCapacity + 5);
        VectorBuffer[VectorSize++] = Value;
    }

    void pop_back() { VectorSize--; }

    void reverse()
    {
        if (VectorSize <= 1)
            return;
        for (uint64_t i = 0, j = VectorSize - 1; i < j; i++, j--)
        {
            T c = *(VectorBuffer + i);
            *(VectorBuffer + i) = *(VectorBuffer + j);
            *(VectorBuffer + j) = c;
        }
    }

    void reserve(uint64_t Capacity)
    {
        if (VectorBuffer == 0)
        {
            VectorSize = 0;
            VectorCapacity = 0;
        }
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: reverse( %ld )", Capacity);
#endif
        T *Newbuffer = new T[Capacity];
        uint64_t _Size = Capacity < VectorSize ? Capacity : VectorSize;
        for (uint64_t i = 0; i < _Size; i++)
            Newbuffer[i] = VectorBuffer[i];
        VectorCapacity = Capacity;
#ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: reverse( <Capacity> )->Buffer:~%ld", VectorBuffer);
#endif
        delete[] VectorBuffer;
        VectorBuffer = Newbuffer;
    }

    void resize(uint64_t Size)
    {
        reserve(Size);
        VectorSize = Size;
    }

    T &operator[](uint64_t Index) { return VectorBuffer[Index]; }

    Vector<T> &operator=(const Vector<T> &Vector)
    {
        delete[] VectorBuffer;
        VectorSize = Vector.VectorSize;
        VectorCapacity = Vector.VectorCapacity;
        #ifdef DEBUG_MEM_ALLOCATION
        debug("VECTOR ALLOCATION: operator=( <vector> )->Size:%ld", VectorSize);
#endif
        VectorBuffer = new T[VectorSize];
        for (uint64_t i = 0; i < VectorSize; i++)
            VectorBuffer[i] = Vector.VectorBuffer[i];
        return *this;
    }

    void clear()
    {
        VectorCapacity = 0;
        VectorSize = 0;
        VectorBuffer = 0;
    }
};
