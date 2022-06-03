#ifndef UNIT_TESTING_H
#define UNIT_TESTING_H

#ifdef UNIT_TESTS

#include <printf.h>

#define TESTING 1

// COM1 0x3F8, COM2 0x2F8, COM3 0x3E8, COM4 0x2E8
#define UNIT_COM 0x3F8

static inline void UNITDirectWriteDebugger(const char *text)
{
    long unsigned text_length = 0;
    while (text[text_length] != '\0')
        ++text_length;
    for (long unsigned i = 0; i < text_length; i++)
    {
    IsTransmitEmpty:
        unsigned char result;
        __asm__("in %%dx, %%al"
                : "=a"(result)
                : "d"(UNIT_COM + 5));
        if ((result & 0x20) == 0)
            goto IsTransmitEmpty;
        __asm__ volatile("out %%al, %%dx"
                         :
                         : "a"(text[i]), "d"(UNIT_COM));
    }
}

static inline void syswritedbgprint_wrapper(char c, void *unused)
{
IsTransmitEmpty:
    unsigned char result;
    __asm__("in %%dx, %%al"
            : "=a"(result)
            : "d"(UNIT_COM + 5));
    if ((result & 0x20) == 0)
        goto IsTransmitEmpty;
    __asm__ volatile("out %%al, %%dx"
                     :
                     : "a"(c), "d"(UNIT_COM));
    (void)unused;
}

static inline int TestDbg(const char *Format, ...)
{
    va_list Args;
    va_start(Args, Format);
    int ret = vfctprintf(syswritedbgprint_wrapper, 0, Format, Args);
    va_end(Args);
    return ret;
}

#define TEST_TEST()                                 \
    UNITDirectWriteDebugger("Testing printf...\n"); \
    TestDbg("Hello World! - %s - %d - %#x - %p\n", "The quick brown fox jumps over the lazy dog", 1234567890, 0xDEADBEEF, &TestDbg)

#define TEST_DBG(Format, ...) TestDbg(Format, ##__VA_ARGS__)

#define TEST_ASSERT(test)                                               \
    if (!(test))                                                        \
    {                                                                   \
        TestDbg("%s:%d: test failed: %s\n", __FILE__, __LINE__, #test); \
        while (1)                                                       \
            __asm__ volatile("hlt");                                    \
    }

#define TEST_EQUAL(expected, actual)                                                         \
    if ((expected) != (actual))                                                              \
    {                                                                                        \
        TestDbg("%s:%d: test failed: %#x != %#x\n", __FILE__, __LINE__, #expected, #actual); \
        while (1)                                                                            \
            __asm__ volatile("hlt");                                                         \
    }

#define TEST_EQUAL_STR(expected, actual)                                                   \
    if (strcmp((expected), (actual)) != 0)                                                 \
    {                                                                                      \
        TestDbg("%s:%d: test failed: %s != %s\n", __FILE__, __LINE__, #expected, #actual); \
        while (1)                                                                          \
            __asm__ volatile("hlt");                                                       \
    }

#define TEST_EQUAL_STRN(expected, actual, length)                                          \
    if (strncmp((expected), (actual), (length)) != 0)                                      \
    {                                                                                      \
        TestDbg("%s:%d: test failed: %s != %s\n", __FILE__, __LINE__, #expected, #actual); \
        while (1)                                                                          \
            __asm__ volatile("hlt");                                                       \
    }

#else

#define TEST_TEST()
#define TEST_ASSERT(test)
#define TEST_EQUAL(expected, actual)
#define TEST_EQUAL_STR(expected, actual)

#endif // UNIT_TESTS

#endif // UNIT_TESTING_H
