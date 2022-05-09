#ifndef __FENNIX_KERNEL_GDT_H__
#define __FENNIX_KERNEL_GDT_H__

#include <types.h>
#include <cputables.h>

#define GDT_KERNEL_CODE offsetof(GlobalDescriptorTableEntries, Code)
#define GDT_KERNEL_DATA offsetof(GlobalDescriptorTableEntries, Data)
#define GDT_USER_DATA (offsetof(GlobalDescriptorTableEntries, UserData) | 3)
#define GDT_USER_CODE (offsetof(GlobalDescriptorTableEntries, UserCode) | 3)
#define GDT_TSS (offsetof(GlobalDescriptorTableEntries, TaskStateSegment) | 3)

extern GlobalDescriptorTableDescriptor gdt;
extern TaskStateSegment *tss;
EXTERNC void init_gdt();
EXTERNC void init_tss();

#endif // !__FENNIX_KERNEL_GDT_H__
