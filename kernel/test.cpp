#include <test.h>

#include "../timer.h"
#include "kernel.h"

#include <internal_task.h>
#include "cpu/gdt.h"
#include "cpu/apic.hpp"
#include "cpu/smp.hpp"
#include <string.h>
#include <heap.h>

void do_libs_test()
{
    TEST_ASSERT(strcmp("String Compare.", "String Compare.") == 0);
    TEST_ASSERT(strncmp("1 Test?", "1 Test?", 0) == 0);
    TEST_ASSERT(memcmp("No, 2 Tests.", "No, 2 Tests.", 0) == 0);
    char buf[12] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    char emptybuf[12];
    memcpy(emptybuf, buf, 12);
    TEST_EQUAL_STRN(emptybuf, buf, 12);
    memset(buf, '\0', 12);
    TEST_EQUAL_STRN("\0\0\0\0\0\0\0\0\0\0\0\0\0", buf, 12);
    memmove(buf, "Hello World!", 12);
    TEST_EQUAL_STRN("Hello World!", buf, 12);
}

void do_mem_test()
{
    uint64_t ReqAddress1 = (uint64_t)KernelAllocator.RequestPage();
    KernelAllocator.FreePage((void *)ReqAddress1);
    for (size_t i = 0; i < 1000; i++)
        KernelAllocator.FreePage(KernelAllocator.RequestPage());
    uint64_t ReqAddress2 = (uint64_t)KernelAllocator.RequestPage();
    TEST_EQUAL(ReqAddress1, ReqAddress2);
    KernelAllocator.FreePage((void *)ReqAddress2);

    uint64_t ReqAddresses1 = (uint64_t)KernelAllocator.RequestPages(10);
    KernelAllocator.FreePages((void *)ReqAddresses1, 10);
    for (size_t i = 0; i < 1000; i++)
        KernelAllocator.FreePages(KernelAllocator.RequestPages(20), 20);
    uint64_t ReqAddresses2 = (uint64_t)KernelAllocator.RequestPages(10);
    TEST_EQUAL(ReqAddresses1, ReqAddresses2);
    KernelAllocator.FreePages((void *)ReqAddresses2, 10);

    uint64_t MallocAddress1 = (uint64_t)kmalloc(0x1000);
    kfree((void *)MallocAddress1);
    for (size_t i = 0; i < 1000; i++)
        kfree(kmalloc(0x10000));
    uint64_t MallocAddress2 = (uint64_t)kmalloc(0x1000);
    TEST_EQUAL(MallocAddress1, MallocAddress2);
    kfree((void *)MallocAddress2);
}

#define srdy 1
#define sliv 2
#define sded 3

struct t_process
{
    uint64_t id;
    int tids;
    REGISTERS regs;
    int status;
};

t_process *processes[256];
t_process *curp = nullptr;

t_process *getnext(t_process *p)
{
    if (p != nullptr)
        for (size_t i = 0; i < 256; i++)
        {
            if (processes[i] == p)
            {
                if (i + 1 < 256)
                {
                    if (processes[i + 1] != nullptr)
                        return processes[i + 1];
                }
            }
        }
    for (size_t i = 0; i < 256; i++)
    {
        if (processes[i] != nullptr)
            return processes[i];
    }
    TEST_DBG("getnext fail\n");
}

void allocproc(t_process *p)
{
    for (size_t i = 0; i < 256; i++)
    {
        if (processes[i] == nullptr)
        {
            processes[i] = p;
            return;
        }
    }
    TEST_DBG("allocproc fail\n");
}

void freeproc(t_process *p)
{
    for (size_t i = 0; i < 256; i++)
    {
        if (processes[i] == p)
        {
            processes[i] = nullptr;
            return;
        }
    }
    TEST_DBG("freeproc fail\n");
}

void safeschedulerhandler(REGISTERS *regs)
{
    if (curp == nullptr)
    {
        curp = getnext(nullptr);
        if (curp == nullptr)
        {
            TEST_DBG("safeschedulerhandler fail\n");
            return;
        }
        *regs = curp->regs;
    }
    else
    {
        curp->regs = *regs;
        t_process *p = getnext(curp);
        if (p == nullptr)
        {
            TEST_DBG("safeschedulerhandler fail\n");
            return;
        }
        curp = p;
        *regs = curp->regs;
    }
    // TEST_DBG("interrupt works\n");
    apic->OneShot(IRQ8, 100);
}

uint64_t ptid = 0;

void leavemealone()
{
    TEST_DBG("process exited\n");
    for (;;)
        ;
}

t_process *cproc(uint64_t rip)
{
    t_process *p = new t_process;
    p->id = ptid++;
    p->tids = 0;
    p->status = srdy;
    p->regs.cs = GDT_KERNEL_CODE;
    p->regs.ss = GDT_KERNEL_DATA;
    p->regs.rflags.always_one = 1;
    p->regs.rflags.IF = 1;
    p->regs.rflags.ID = 1;
    void *stack = KernelAllocator.RequestPage();
    p->regs.STACK = (uint64_t)stack;
    POKE(uint64_t, p->regs.rsp) = (uint64_t)leavemealone;

    p->regs.FUNCTION = (uint64_t)rip;
    p->regs.ARG0 = (uint64_t)p->id;
    p->regs.ARG1 = (uint64_t)p->id + 1;
    allocproc(p);
    return p;
}

void test_safeschedulerproc(int id, int idk)
{
    int stop = 10000;
    while (stop--)
    {
        TEST_DBG("%d", id);
    }
}

void test_safescheduler()
{
    TEST_DBG("Proceeding to use safe scheduler\n");
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    cproc((uint64_t)test_safeschedulerproc);
    register_interrupt_handler(IRQ8, safeschedulerhandler);
    apic->RedirectIRQ(CurrentCPU->ID, IRQ8 - 32, 1);
    apic->OneShot(IRQ8, 100);
}

using namespace Tasking;

void test_stress_task(int a, int b)
{
    TEST_DBG("T->%s(%d)[%d,%d]\n", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, a, b);
    sleep(10);
}

void test_kernelmultitasking(int a, int b)
{
    TEST_EQUAL(a, b);
    TEST_DBG("Multitasking test started.\n");

    PCB *pcb1 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb2 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb3 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb4 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);

    size_t threads = 2;

    for (size_t i = 0; i < threads; i++)
    {
        mt->CreateThread(pcb1, (uint64_t)test_stress_task, 1, i);
    }

    threads *= 2;
    for (size_t i = 0; i < threads; i++)
    {
        mt->CreateThread(pcb2, (uint64_t)test_stress_task, 2, i);
    }

    threads *= 2;
    for (size_t i = 0; i < threads; i++)
    {
        mt->CreateThread(pcb3, (uint64_t)test_stress_task, 3, i);
    }

    threads *= 2;
    for (size_t i = 0; i < threads; i++)
    {
        mt->CreateThread(pcb4, (uint64_t)test_stress_task, 4, i);
    }

    TEST_DBG("Multitasking test finished.\n");
    MultitaskingSchedulerEnabled = false;
    delete mt;
    if (sysflags->monotasking)
        StartTasking((uint64_t)KernelTask, TaskingMode::Mono);
    else
        StartTasking((uint64_t)KernelTask, TaskingMode::Multi);
}

void do_tasking_test()
{
    test_safescheduler();
    // mt = new Multitasking;
    // mt->CreateThread(mt->CreateProcess(nullptr, (char *)"FakeKernel", ELEVATION::Kernel), (uint64_t)test_kernelmultitasking, 1998, 1998);
    // MultitaskingSchedulerEnabled = true;
}
