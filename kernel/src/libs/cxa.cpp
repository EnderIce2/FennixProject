#include "cxa.h"
#include <debug.h>

// TODO: https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html
// TODO: https://wiki.osdev.org/C%2B%2B

extern "C" void *__dso_handle = 0;
atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

extern "C" int __cxa_atexit(void (*f)(void *), void *objptr, void *dso)
{
    debug("__cxa_atexit triggered.");
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS)
    {
        return -1;
    };
    __atexit_funcs[__atexit_func_count].destructor_func = f;
    __atexit_funcs[__atexit_func_count].obj_ptr = objptr;
    __atexit_funcs[__atexit_func_count].dso_handle = dso;
    __atexit_func_count++;
    return 0;
};

extern "C" void __cxa_finalize(void *f)
{
    debug("__cxa_finalize triggered.");
    uarch_t i = __atexit_func_count;
    if (!f)
    {
        while (i--)
        {
            if (__atexit_funcs[i].destructor_func)
            {
                (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            };
        };
        return;
    };

    while (i--)
    {
        if (__atexit_funcs[i].destructor_func == f)
        {
            (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            __atexit_funcs[i].destructor_func = 0;
        };
    };
};

extern "C" _Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action actions, _Unwind_Exception_Class exception_class, struct _Unwind_Exception *ue_header, struct _Unwind_Context *context)
{
    (void)(version);
    (void)(actions);
    (void)(exception_class);
    (void)(ue_header);
    (void)(context);
    debug("__gxx_personality_v0 triggered.");
    return _URC_NO_REASON;
}

extern "C" void _Unwind_Resume(struct _Unwind_Exception *exc)
{
    (void)(exc);
    debug("_Unwind_Resume triggered.");
}

extern "C" void *__cxa_allocate_exception(size_t thrown_size) throw()
{
    (void)(thrown_size);
    debug("__cxa_allocate_exception triggered.");
    return (void *)0;
}

extern "C" void __cxa_throw(void *thrown_object, void *tinfo, void (*dest)(void *))
{
    (void)(thrown_object);
    (void)(tinfo);
    (void)(dest);
    debug("__cxa_throw triggered.");
}

extern "C" void __cxa_rethrow() { debug("__cxa_rethrow triggered."); }

extern "C" void __cxa_pure_virtual() { debug("__cxa_pure_virtual triggered."); }
