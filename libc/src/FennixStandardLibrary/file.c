#include <file.h>

#include <alloc.h>
#include <syscalls.h>

#include <system.h>

FILE *FileOpen(const char *Filename, const char *Mode)
{
    void *ret = (void *)syscall_FileOpen((char *)Filename /*, Mode*/);
    if (ret == NULL)
        return NULL;
    FILE *file = (FILE *)malloc(sizeof(FILE));
    file->Khnd = ret;
    return file;
}

size_t FileRead(void *Buffer, size_t Size, size_t Count, FILE *Stream)
{
    return syscall_FileRead(Stream->Khnd, 0 /*offset*/, Buffer, Size);
}

size_t FileWrite(const void *Buffer, size_t Size, size_t Count, FILE *Stream)
{
    return syscall_FileWrite(Stream->Khnd, 0 /*offset*/, (void *)Buffer, Size);
}

int FileSeek(FILE *Stream, long int Offset, enum Seek Whence)
{
    WriteSysDebugger("FileSeek( %p %ld %d ) stub\n", Stream, Offset, Whence);
    return 0;
}

long int FileTell(FILE *Stream)
{
    WriteSysDebugger("FileTell( %p ) stub\n", Stream);
    return 0;
}

int MakeDirectory(const char *Path, int Mode)
{
    WriteSysDebugger("MakeDirectory( %s %d ) stub\n", Path, Mode);
    return 0;
}

int RenameFile(const char *OldFileName, const char *NewFileName)
{
    WriteSysDebugger("RenameFile( %s %s ) stub\n", OldFileName, NewFileName);
    return 0;
}

int RemoveFile(const char *FileName)
{
    WriteSysDebugger("RemoveFile( %s ) stub\n", FileName);
    return 0;
}

int FileClose(FILE *Stream)
{
    syscall_FileClose(Stream->Khnd);
    free(Stream);
    return 0;
}
