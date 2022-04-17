#pragma once

#include <types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /** This is a boundary tag which is prepended to the
     * page or section of a page which we have allocated. It is
     * used to identify valid memory blocks that the
     * application is trying to free.
     */
    struct boundary_tag
    {
        unsigned int magic;     //< It's a kind of ...
        unsigned int size;      //< Requested size.
        unsigned int real_size; //< Actual size.
        int index;              //< Location in the page table.

        struct boundary_tag *split_left;  //< Linked-list info for broken pages.
        struct boundary_tag *split_right; //< The same.

        struct boundary_tag *next; //< Linked list info.
        struct boundary_tag *prev; //< Linked list info.
    };

    void *liballoc_malloc(size_t);          //< The standard function.
    void *liballoc_realloc(void *, size_t); //< The standard function.
    void *liballoc_calloc(size_t, size_t);  //< The standard function.
    void liballoc_free(void *);             //< The standard function.

#ifdef __cplusplus
}
#endif

