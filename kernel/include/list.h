#ifndef __FENNIX_KERNEL_LIST_H__
#define __FENNIX_KERNEL_LIST_H__

#include <types.h>

struct list_head
{
    struct list_head *next, *prev;
};

#ifndef __cplusplus

#define LIST_HEAD_INIT(name) \
    {                        \
        &(name), &(name)     \
    }

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)  \
    do                       \
    {                        \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
    } while (0)

static inline void list_init(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *add, struct list_head *prev, struct list_head *next)
{
    next->prev = add;
    add->next = next;
    add->prev = prev;
    prev->next = add;
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_add(struct list_head *add, struct list_head *head)
{
    __list_add(add, head, head->next);
}

static inline void list_add_tail(struct list_head *add, struct list_head *head)
{
    __list_add(add, head->prev, head);
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static inline void list_replace(struct list_head *old, struct list_head *add)
{
    add->next = old->next;
    add->next->prev = add;
    add->prev = old->prev;
    add->prev->next = add;
}

static inline void list_splice(struct list_head *list, struct list_head *head)
{
    struct list_head *first = list->next;
    if (first != list)
    {
        struct list_head *last = list->prev;
        struct list_head *at = head->next;
        first->prev = head;
        head->next = first;
        last->next = at;
        at->prev = last;
    }
}

static inline int list_is_first(const struct list_head *list, const struct list_head *head)
{
    return list->prev == head;
}

static inline int list_is_last(const struct list_head *list, const struct list_head *head)
{
    return list->next == head;
}

static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}

#define list_entry(ptr, type, member) ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, pnext, head) \
    for (pos = (head)->next, pnext = pos->next; pos != (head); pos = pnext, pnext = pos->next)

#define list_next_entry(pos, member) \
    list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_for_each_entry_continue(pos, head, member) \
    for (pos = list_next_entry(pos, member);            \
         &pos->member != (head);                        \
         pos = list_next_entry(pos, member))

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_for_each_entry(pos, head, member)               \
    for (pos = list_first_entry(head, typeof(*pos), member); \
         &pos->member != (head);                             \
         pos = list_next_entry(pos, member))

#define list_for_each_entry_safe(pos, n, head, member)       \
    for (pos = list_first_entry(head, typeof(*pos), member), \
        n = list_next_entry(pos, member);                    \
         &pos->member != (head);                             \
         pos = n, n = list_next_entry(n, member))

#endif

#endif // !__FENNIX_KERNEL_LIST_H__