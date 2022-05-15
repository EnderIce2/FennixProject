#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    enum debug_type
    {
        _default_val,
        _fixme,
        _stub,
        _trace,
        _warn,
        _err,
        _debug,
        _checkpoint,
        _ubsan
    };
#ifdef DEBUG
/**
 * @brief Log stub
 *
 */
#define fixme(m, ...) debug_printf(_fixme,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Log stub
 *
 */
#define stub debug_printf(_stub, null, __FILE__, __LINE__, __FUNCTION__)
/**
 * @brief Log trace
 *
 */
#define trace(m, ...) debug_printf(_trace,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Log warning
 *
 */
#define warn(m, ...) debug_printf(_warn,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Log error
 *
 */
#define err(m, ...) debug_printf(_err,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Log debug
 *
 */
#define debug(m, ...) debug_printf(_debug,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Log debug
 *
 */
#define ubsan(m, ...) debug_printf(_ubsan,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
/**
 * @brief Writes in the debugger CHECKPOINT message.
 *
 */
#define CHECKPOINT(...) debug_printf(checkpoint, null, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define TRACEREGS(r) dbg_showregs(r, __FILE__, __LINE__, __FUNCTION__)

#else
#define fixme(m, ...) (void)(m)
#define stub ;
#define trace(m, ...) (void)(m)
#define warn(m, ...) debug_printf(warn,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
#define err(m, ...) debug_printf(err,__FILE__, __LINE__, __FUNCTION__, m, ##__VA_ARGS__)
#define debug(m, ...) (void)(m)
#define ubsan(m, ...) (void)(m)
#define TRACEREGS(r) (void)(r)
#endif

    void dbg_showregs(void *regs, const char *file, int line, const char *function);
    int debug_printf(enum debug_type type, const char *file, int line, const char *function, const char *message, ...);

#ifdef __cplusplus
}
#endif