extern "C" void *malloc(unsigned long s);
extern "C" void free(void *p);
extern "C" void *memset(void *dest, int data, unsigned long nbytes);

void *operator new(unsigned long Size)
{
    if (Size == 0)
        Size = 1;
    if (void *Pointer = malloc(Size))
    {
        memset(Pointer, 0, Size);
        return Pointer;
    }
    // warn("new( %llx ) failed!", Size);
    throw;
}

void *operator new[](unsigned long Size)
{
    if (Size == 0)
        Size = 1;
    if (void *Pointer = malloc(Size))
    {
        memset(Pointer, 0, Size);
        return Pointer;
    }
    // warn("new[]( %llx ) failed!", Size);
    throw;
}

void operator delete(void *Pointer)
{
    free(Pointer);
}

void operator delete[](void *Pointer)
{
    free(Pointer);
}

void operator delete(void *Pointer, long unsigned int n)
{
    free(Pointer);
}

void operator delete[](void *Pointer, long unsigned int n)
{
    free(Pointer);
}
