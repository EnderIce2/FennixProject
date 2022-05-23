#pragma once

#define NO_RETURN __attribute__((__noreturn__))
#define NO_THROW __attribute__((__nothrow__))
#define PURE __attribute__((__pure__))
#define NONNULL(params) __attribute__((__nonnull__ params))
#define RESTRICT __restrict

#define ArgumentPack() __builtin_va_arg_pack()
#define ArgumentPackLength() __builtin_va_arg_pack_len()

#ifdef __cplusplus

#define E extern "C"
#define SEC    \
    extern "C" \
    {
#define EEC \
    }

#else

#define E extern
#define SEC // Start Extern C
#define EEC // End Extern C

#endif
