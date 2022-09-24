#include <test.h>

#include "network/NetworkController.hpp"
#include "cpu/apic.hpp"
#include "cpu/smp.hpp"
#include "../timer.h"
#include "cpu/gdt.h"
#include "kernel.h"

#include <internal_task.h>
#include <string.h>
#include <heap.h>

void do_libs_test()
{
#ifdef UNIT_TESTS
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
#endif
}

void do_mem_bitmap_print()
{
#ifdef UNIT_TESTS
    TEST_DBG("\nPrinting Bitmap:\n");
    uint64_t nl = 1;
    for (uint64_t i = 0; i < KernelAllocator.PageBitmap.Size; i++)
    {
        nl--;
        if (nl <= 0)
        {
            nl = 148;
            TEST_DBG("\n%08d: ", i);
        }
        TEST_DBG("%d", KernelAllocator.PageBitmap[i] ? 1 : 0);
    }
    TEST_DBG("\n");
#endif
}

#ifdef UNIT_TESTS
class test_mem_new_delete
{
public:
    test_mem_new_delete();
    ~test_mem_new_delete();
};

test_mem_new_delete::test_mem_new_delete()
{
    for (char i = 0; i < 2; i++)
        ;
}

test_mem_new_delete::~test_mem_new_delete()
{
    for (char i = 0; i < 2; i++)
        ;
}

#define MEMTEST_ITERATIONS 1024

#endif

void do_mem_test()
{
#ifdef UNIT_TESTS
    // return;

    void *tmpAlloc1 = kmalloc(176);
    void *tmpAlloc2 = kmalloc(511);
    void *tmpAlloc3 = kmalloc(1027);
    void *tmpAlloc4 = kmalloc(1569);
    TEST_DBG("Kernel Address: Start:%p ---- End:%p [%ldKB/%ldKB]\n", bootparams->Kernel.File, bootparams->Kernel.File + bootparams->Kernel.Length, TO_KB(KernelAllocator.GetUsedRAM()), TO_KB(KernelAllocator.GetFreeRAM()));
    for (int repeat = 0; repeat < 16; repeat++)
    {
        TEST_DBG("---------------[TEST %d]---------------\n", repeat);

        TEST_DBG("Single Page Request Test");

        uint64_t ReqAddress1 = (uint64_t)KernelAllocator.RequestPage();
        KernelAllocator.FreePage((void *)ReqAddress1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
            KernelAllocator.FreePage(KernelAllocator.RequestPage());

        uint64_t ReqAddress2 = (uint64_t)KernelAllocator.RequestPage();
        KernelAllocator.FreePage((void *)ReqAddress2);

        TEST_DBG(" Result:\t\t1-[%#lx]; 2-[%#lx]\n", (void *)ReqAddress1, (void *)ReqAddress2);
        TEST_EQUAL(ReqAddress1, ReqAddress2);

        // Next Test

        TEST_DBG("Multiple Page Request Test");

        uint64_t ReqAddresses1 = (uint64_t)KernelAllocator.RequestPages(10);
        KernelAllocator.FreePages((void *)ReqAddresses1, 10);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
            KernelAllocator.FreePages(KernelAllocator.RequestPages(20), 20);

        uint64_t ReqAddresses2 = (uint64_t)KernelAllocator.RequestPages(10);
        KernelAllocator.FreePages((void *)ReqAddresses2, 10);

        TEST_DBG(" Result:\t\t1-[%#lx]; 2-[%#lx]\n", (void *)ReqAddresses1, (void *)ReqAddresses2);
        TEST_EQUAL(ReqAddresses1, ReqAddresses2);

        // Next Test

        TEST_DBG("Multiple Fixed Malloc Test");

        uint64_t MallocAddress1 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
            kfree(kmalloc(0x10000));

        uint64_t MallocAddress2 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress2);

        TEST_DBG(" Result:\t\t1-[%#lx]; 2-[%#lx]\n", (void *)MallocAddress1, (void *)MallocAddress2);
        TEST_EQUAL(MallocAddress1, MallocAddress2);

        // Next Test

        TEST_DBG("Multiple Dynamic Malloc Test");

        uint64_t MallocAddress_1 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress_1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
            kfree(kmalloc(i));

        uint64_t MallocAddress_2 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress_2);

        TEST_DBG(" Result:\t1-[%#lx]; 2-[%#lx]\n", (void *)MallocAddress_1, (void *)MallocAddress_2);
        TEST_EQUAL(MallocAddress_1, MallocAddress_2);

        // Next Test

        TEST_DBG("New/Delete Test");

        uint64_t MallocAddress__1 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress__1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
        {
            test_mem_new_delete *t = new test_mem_new_delete();
            delete t;
        }

        uint64_t MallocAddress__2 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress__2);

        TEST_DBG(" Result:               \t1-[%#lx]; 2-[%#lx]\n", (void *)MallocAddress__1, (void *)MallocAddress__2);
        TEST_EQUAL(MallocAddress__1, MallocAddress__2);

        // Next Test

        TEST_DBG("New/Delete Fixed Array Test");

        uint64_t MallocAddress___1 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress___1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
        {
            char *t = new char[128];
            delete[] t;
        }

        uint64_t MallocAddress___2 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress___2);

        TEST_DBG(" Result:    \t1-[%#lx]; 2-[%#lx]\n", (void *)MallocAddress___1, (void *)MallocAddress___2);
        TEST_EQUAL(MallocAddress___1, MallocAddress___2);

        // Next Test

        TEST_DBG("New/Delete Dynamic Array Test");

        uint64_t MallocAddress____1 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress____1);

        for (uint64_t i = 0; i < MEMTEST_ITERATIONS; i++)
        {
            if (i == 0)
                continue;
            char *t = new char[i];
            delete[] t;
        }

        uint64_t MallocAddress____2 = (uint64_t)kmalloc(0x1000);
        kfree((void *)MallocAddress____2);

        TEST_DBG(" Result:\t1-[%#lx]; 2-[%#lx]\n", (void *)MallocAddress____1, (void *)MallocAddress____2);
        TEST_EQUAL(MallocAddress____1, MallocAddress____2);
    }

    kfree(tmpAlloc1);
    kfree(tmpAlloc2);
    kfree(tmpAlloc3);
    kfree(tmpAlloc4);
    do_mem_bitmap_print();
#endif
}

#ifdef UNIT_TESTS
InterruptHandler(stub_int_hnd)
{
    TEST_DBG("Int-%#x-hnd ", INT_NUM);
    return;
}
#endif

extern "C" void do_interrupts_mem_test()
{
#ifdef UNIT_TESTS
    kmalloc(176);
    kmalloc(511);
    kmalloc(1027);
    kmalloc(1569);
    if (InterruptsEnabled())
        CLI;
    TEST_DBG("Testing memory without interrupts...\n");
    do_mem_test();
    RegisterInterrupt(stub_int_hnd, IRQ10, true);
    for (uint64_t i = 0; i < 256; i++)
        asm("int $0x2a");

    TEST_DBG("\nTesting interrupt handler again but with sti...\n");
    if (!InterruptsEnabled())
        STI;
    for (uint64_t i = 0; i < 256; i++)
        asm("int $0x2a");
    UnregisterInterrupt(IRQ10);
    TEST_DBG("Now testing memory without interrupts...\n");
    do_mem_test();
    do_mem_test();
    do_mem_test();
    do_mem_test();
    do_mem_test();
    do_mem_test();
    TEST_DBG("\n\n\n\n\n========================================================================================\nTEST COMPLETE! HALTING...\n========================================================================================\n");
    CPU_HALT;
#endif
}

#ifdef UNIT_TESTS

#define srdy 1
#define sliv 2
#define sded 3

struct t_process
{
    uint64_t id;
    int tids;
    TrapFrame regs;
    int status;
};

t_process *processes[256];
t_process *curp = nullptr;

t_process *getnext(t_process *p)
{
    if (p != nullptr)
        for (uint64_t i = 0; i < 256; i++)
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
    for (uint64_t i = 0; i < 256; i++)
    {
        if (processes[i] != nullptr)
            return processes[i];
    }
    TEST_DBG("getnext fail\n");
    return nullptr;
}

void allocproc(t_process *p)
{
    for (uint64_t i = 0; i < 256; i++)
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
    for (uint64_t i = 0; i < 256; i++)
    {
        if (processes[i] == p)
        {
            processes[i] = nullptr;
            return;
        }
    }
    TEST_DBG("freeproc fail\n");
}

void safeschedulerhandler(TrapFrame *regs)
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
    RegisterInterrupt(safeschedulerhandler, IRQ8, true, true);
    apic->OneShot(IRQ8, 100);
}

using namespace Tasking;

void test_stress_task(int a, int b)
{
    if (a == 69)
        TEST_DBG("\n");
    else
        TEST_DBG("T->%s(%d)[%d,%d]\n", SysGetCurrentThread()->Name, SysGetCurrentThread()->ID, a, b);
    TEST_DBG("5\n");
    sleep(1);
    TEST_DBG("4\n");
    sleep(1);
    TEST_DBG("3\n");
    sleep(1);
    TEST_DBG("2\n");
    sleep(1);
    TEST_DBG("1\n");
    sleep(1);
    TEST_DBG("bye\n");
}

void test_kernelmultitasking(int a, int b)
{
    TEST_EQUAL(a, b);
    TEST_DBG("Multitasking test started.\n");

    PCB *pcb1 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb2 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb3 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);
    PCB *pcb4 = mt->CreateProcess(nullptr, (char *)"test", ELEVATION::Kernel);

    uint64_t threads = 2;

    for (uint64_t i = 0; i < threads; i++)
        mt->CreateThread(pcb1, (uint64_t)test_stress_task, 1, i);

    threads *= 2;
    for (uint64_t i = 0; i < threads; i++)
        mt->CreateThread(pcb2, (uint64_t)test_stress_task, 2, i);

    threads *= 2;
    for (uint64_t i = 0; i < threads; i++)
        mt->CreateThread(pcb3, (uint64_t)test_stress_task, 3, i);

    threads *= 2;
    for (uint64_t i = 0; i < threads; i++)
        mt->CreateThread(pcb4, (uint64_t)test_stress_task, 4, i);

    mt->CreateThread(pcb4, (uint64_t)test_stress_task, 69, 0);

    TEST_DBG("Multitasking test finished.\n");
}

#endif

void do_tasking_test()
{
#ifdef UNIT_TESTS
    // test_safescheduler();
    StartTasking((uint64_t)test_kernelmultitasking, TaskingMode::Multi);
    CPU_STOP;
#endif
}

#ifdef UNIT_TESTS
void test_stack_final()
{
    // void *p = (void *)0x567777756886;
    // ((void(*)())p)();
    // asm("int $0xd");
}
void backtrace9() { test_stack_final(); }
void backtrace8() { backtrace9(); }
void backtrace7() { backtrace8(); }
void backtrace6() { backtrace7(); }
void backtrace5() { backtrace6(); }
void backtrace4() { backtrace5(); }
void backtrace3() { backtrace4(); }
void backtrace2() { backtrace3(); }
void backtrace1() { backtrace2(); }
void backtrace0() { backtrace1(); }
#endif

void do_stacktrace_test()
{
#ifdef UNIT_TESTS
    backtrace0();
#endif
}

void do_network_test()
{
#ifdef UNIT_TESTS
    TEST_DBG("Network test started.\n");
    InternetProtocol4 IP1 = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};
    InternetProtocol4 IP2 = {.Address = {0xFF, 0xFF, 0xFF, 0xFF}};

    InternetProtocol4 IPN1 = {.Address = {127, 0, 0, 1}};
    InternetProtocol4 IPN2 = {.Address = {192, 168, 0, 1}};

    MediaAccessControl MAC1 = {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    MediaAccessControl MAC2 = {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    MediaAccessControl MACN1 = {.Address = {0xF0, 0x2F, 0x6F, 0xF7, 0x1F, 0xFF}};
    MediaAccessControl MACN2 = {.Address = {0xF2, 0xAF, 0x02, 0xF1, 0x1C, 0xFA}};

    MediaAccessControl MACV1 = {.Address = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    MediaAccessControl MACV2 = {.Address = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}};

    TEST_ASSERT(IP1 == IP2);
    TEST_ASSERT(IPN1 != IPN2);
    TEST_ASSERT(IP1.ToHex() == 0xFFFFFFFF)
    TEST_ASSERT(IP1.FromHex(0xFFFFFFFF) == IP2);

    TEST_ASSERT(MAC1 == MAC2);
    TEST_ASSERT(MACN1 != MACN2);
    TEST_ASSERT(MAC1.ToHex() == 0xFFFFFFFFFFFF);
    TEST_ASSERT(MAC1.FromHex(0xFFFFFFFFFFFF) == MAC2);
    TEST_ASSERT(MACV1.Valid() == false);
    TEST_ASSERT(MACV2.Valid() == false);
    TEST_ASSERT(MACN1.Valid() == true);
    TEST_ASSERT(MACN2.Valid() == true);

    TEST_DBG("Network test finished.\n");
#endif
}
