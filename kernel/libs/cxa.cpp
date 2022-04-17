#include <types.h>
#include <debug.h>

// TODO: complete implementation for everything
// TODO: https://wiki.osdev.org/C%2B%2B

#define ATEXIT_MAX_FUNCS 128

typedef unsigned uarch_t;

struct atexit_func_entry_t
{
    /*
     * Each member is at least 4 bytes large. Such that each entry is 12bytes.
     * 128 * 12 = 1.5KB exact.
     **/
    void (*destructor_func)(void *);
    void *obj_ptr;
    void *dso_handle;
};

typedef enum
{
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

struct _Unwind_Context;
typedef unsigned _Unwind_Exception_Class __attribute__((__mode__(__DI__)));
typedef unsigned _Unwind_Word __attribute__((__mode__(__unwind_word__)));
typedef void (*_Unwind_Exception_Cleanup_Fn)(_Unwind_Reason_Code, struct _Unwind_Exception *);
typedef int _Unwind_Action;

struct _Unwind_Exception
{
    _Unwind_Exception_Class exception_class;
    _Unwind_Exception_Cleanup_Fn exception_cleanup;
#if !defined(__USING_SJLJ_EXCEPTIONS__) && defined(__SEH__)
    _Unwind_Word private_[6];
#else
    _Unwind_Word private_1;
    _Unwind_Word private_2;
#endif
} __attribute__((__aligned__));

extern void *__dso_handle = 0;
atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

extern "C" int __cxa_atexit(void (*f)(void *), void *objptr, void *dso)
{
    fixme("__cxa_atexit( %p %p %p ) triggered.", f, objptr, dso);
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS)
    {
        return -1;
    }
    __atexit_funcs[__atexit_func_count].destructor_func = f;
    __atexit_funcs[__atexit_func_count].obj_ptr = objptr;
    __atexit_funcs[__atexit_func_count].dso_handle = dso;
    __atexit_func_count++;
    return 0;
}

extern "C" void __cxa_finalize(void *f)
{
    fixme("__cxa_finalize( %p ) triggered.", f);
    uarch_t i = __atexit_func_count;
    if (!f)
    {
        while (i--)
        {
            if (__atexit_funcs[i].destructor_func)
            {
                (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            }
        }
        return;
    }

    while (i--)
    {
        if (__atexit_funcs[i].destructor_func == f)
        {
            (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            __atexit_funcs[i].destructor_func = 0;
        }
    }
}

extern "C" _Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions, _Unwind_Exception_Class exception_class, _Unwind_Exception *ue_header, _Unwind_Context *context)
{
    fixme("__gxx_personality_v0( %d %p %p %p %p ) triggered.", version, actions, exception_class, ue_header, context);
    return _URC_NO_REASON;
}

extern "C" void _Unwind_Resume(struct _Unwind_Exception *exc)
{
    fixme("_Unwind_Resume( %p ) triggered.", exc);
}

extern "C" void *__cxa_allocate_exception(uint64_t thrown_size) throw()
{
    fixme("__cxa_allocate_exception( %#llu ) triggered.", thrown_size);
    return (void *)0;
}

extern "C" void __cxa_throw(void *thrown_object, void *tinfo, void (*dest)(void *))
{
    fixme("__cxa_throw( %p %p %p ) triggered.", thrown_object, tinfo, dest);
}

extern "C" void __cxa_rethrow()
{
    fixme("__cxa_rethrow() triggered.");
}

extern "C" void __cxa_pure_virtual()
{
    fixme("__cxa_pure_virtual() triggered.");
}

extern "C" void __cxa_throw_bad_array_new_length()
{
    fixme("__cxa_throw_bad_array_new_length() triggered.");
}

extern "C" void __cxa_free_exception(void *thrown_exception)
{
    fixme("__cxa_free_exception( %p ) triggered.", thrown_exception);
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
extern "C" void *__cxa_begin_catch(void *e) throw()
#else
extern "C" void *__cxa_begin_catch(void *e)
#endif
{
    fixme("__cxa_begin_catch( %p ) triggered.", e);
    return (void *)0;
}

extern "C" void __cxa_end_catch()
{
    fixme("__cxa_end_catch() triggered.");
}
