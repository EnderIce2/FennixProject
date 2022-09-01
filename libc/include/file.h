#ifndef _FSL_FILE_H
#define _FSL_FILE_H

#include <cdefs.h>
#include <stddef.h>

typedef struct _FILE
{
    /** @brief Kernel internal file handle. Trying to access it will cause a page fault. */
    void *Khnd;
    char PrintBuffer[2048];
} FILE;

enum Seek
{
    SEEK_BEGIN,
    SEEK_CURRENT,
    SEEK_END
};

/**
 * @brief Open a file specified by @ref filename and @ref mode.
 *
 * @param Filename The filename to open.
 * @param Mode The mode to open the file in.
 * @return The file pointer to the opened file.
 */
E FILE *FileOpen(const char *Filename, const char *Mode);

/**
 * @brief Read data from the given stream into the array pointed to, by @ref Buffer.
 *
 * @param Buffer The array to read data into with a minimum size of @ref Size * @ref Count.
 * @param Size The size of each element to read.
 * @param Count The number of elements to read (each element is @ref Size bytes long).
 * @param Stream The stream to read from (obtained from @ref FileOpen).
 * @return The number of bytes read into the array.
 */
E size_t FileRead(unsigned char *Buffer, size_t Size, size_t Count, FILE *Stream);

/**
 * @brief Write data from the array pointed to by @ref Buffer to the given stream.
 *
 * @param Buffer The array to write data from.
 * @param Size The size of each element to write.
 * @param Count The number of elements to write (each element is @ref Size bytes long).
 * @param Stream The stream to write to (obtained from @ref FileOpen).
 * @return The number of bytes written to the stream.
 */
E size_t FileWrite(const unsigned char *Buffer, size_t Size, size_t Count, FILE *Stream);

E int FileSeek(FILE *Stream, long int Offset, enum Seek Whence);

E long int FileTell(FILE *Stream);

E int MakeDirectory(const char *Path, int Mode);

E int RenameFile(const char *OldFileName, const char *NewFileName);

E int RemoveFile(const char *FileName);

/**
 * @brief Close the given stream.
 *
 * @param stream The stream to close.
 * @return 0 on success.
 */
E int FileClose(FILE *Stream);

#endif
