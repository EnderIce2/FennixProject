#include "ubsan.h"

// TODO: implement these functions:
/*
__ubsan_handle_type_mismatch_v1_abort
__ubsan_handle_add_overflow_abort
__ubsan_handle_sub_overflow_abort
__ubsan_handle_mul_overflow_abort
__ubsan_handle_negate_overflow_abort
__ubsan_handle_divrem_overflow_abort
__ubsan_handle_shift_out_of_bounds_abort
__ubsan_handle_out_of_bounds_abort
__ubsan_handle_vla_bound_not_positive_abort
__ubsan_handle_float_cast_overflow
__ubsan_handle_float_cast_overflow_abort
__ubsan_handle_load_invalid_value_abort
__ubsan_handle_invalid_builtin_abort
__ubsan_handle_function_type_mismatch_abort
__ubsan_handle_nonnull_return_v1
__ubsan_handle_nonnull_return_v1_abort
__ubsan_handle_nullability_return_v1
__ubsan_handle_nullability_return_v1_abort
__ubsan_handle_nonnull_arg_abort
__ubsan_handle_nullability_arg
__ubsan_handle_nullability_arg_abort
__ubsan_handle_pointer_overflow_abort
__ubsan_handle_cfi_check_fail
*/

// Alignment must be a power of 2.
#define is_aligned(value, alignment) !(value & (alignment - 1))

#ifdef DEBUG

const char *Type_Check_Kinds[] = {
    "Load of",
    "Store to",
    "Reference binding to",
    "Member access within",
    "Member call on",
    "Constructor call on",
    "Downcast of",
    "Downcast of",
    "Upcast of",
    "Cast to virtual base of",
};

// Prevent huge spam from ubsan
bool showubsanmsg(const char *file, uint32_t line, uint32_t column)
{
    // blacklist
    if (strstr(file, "liballoc"))
        return false;

    static char *onceFile[512] = {"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};
    static uint32_t onceLine[512] = {0};
    static uint32_t onceColumn[512] = {0};
    static uint32_t onceCount = 0;

    for (uint32_t i = 0; i < onceCount; i++)
    {
        if ((!strcmp(onceFile[i], file)) && onceLine[i] == line && onceColumn[i] == column)
            return false;
    }
    onceFile[onceCount] = (char *)file;
    onceLine[onceCount] = line;
    onceColumn[onceCount] = column;
    ubsan("\t\tIn File: %s:%i:%i", file, line, column);
    onceCount++;
    return true;
}

// __ubsan_handle_type_mismatch is renamed to __ubsan_handle_type_mismatch_v1 in 2017 beacuse of compatibility or idk

void __ubsan_handle_type_mismatch_v1(struct type_mismatch_v1_data *type_mismatch, uintptr_t pointer)
{
    struct source_location *location = &type_mismatch->location;
    if (pointer == 0)
    {
        if (showubsanmsg(location->file, location->line, location->column))
            ubsan("Null pointer access.");
    }
    else if (type_mismatch->alignment != 0 && is_aligned(pointer, type_mismatch->alignment))
    {
        if (showubsanmsg(location->file, location->line, location->column))
            ubsan("Unaligned memory access %#llx.", pointer);
    }
    else
    {
        if (showubsanmsg(location->file, location->line, location->column))
            ubsan("%s address %#llx with insufficient space for object of type %s",
                  Type_Check_Kinds[type_mismatch->type_check_kind], (void *)pointer, type_mismatch->type->name);
    }
}

void __ubsan_handle_add_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Addition overflow.");
}

void __ubsan_handle_sub_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Subtraction overflow.");
}

void __ubsan_handle_mul_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Multiplication overflow.");
}

void __ubsan_handle_divrem_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Division overflow.");
}

void __ubsan_handle_negate_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Negation overflow.");
}

void __ubsan_handle_pointer_overflow(struct overflow_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Pointer overflow.");
}

void __ubsan_handle_shift_out_of_bounds(struct shift_out_of_bounds_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Shift out of bounds.");
}

void __ubsan_handle_load_invalid_value(struct invalid_value_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Invalid load value.");
}

void __ubsan_handle_out_of_bounds(struct array_out_of_bounds_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Array out of bounds.");
}

void __ubsan_handle_vla_bound_not_positive(struct negative_vla_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Variable-length argument is negative.");
}

void __ubsan_handle_nonnull_return(struct nonnull_return_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Non-null return is null.");
}

void __ubsan_handle_nonnull_return_v1(struct nonnull_return_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Non-null return is null.");
}

void __ubsan_handle_nonnull_arg(struct nonnull_arg_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Non-null argument is null.");
}

void __ubsan_handle_builtin_unreachable(struct unreachable_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Unreachable code reached.");
}

void __ubsan_handle_invalid_builtin(struct invalid_builtin_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Invalid builtin.");
}

void __ubsan_handle_missing_return(struct unreachable_data *data)
{
    if (showubsanmsg(data->location.file, data->location.line, data->location.column))
        ubsan("Missing return.");
}

#endif
