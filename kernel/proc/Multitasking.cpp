#include <internal_task.h>

#include "../cpu/apic.hpp"
#include "../cpu/gdt.h"
#include "fxsr.h"
#include "../timer.h"

#include <interrupts.h>
#include <critical.hpp>
#include <debug.h>
#include <rand.h>
#include <sys.h>
#include <int.h>
#include <asm.h>
#include <io.h>

// IRQ10 == 0x2a
#define SchedulerInterrupt IRQ10

// #define DEBUG_SCHEDULER 1

#ifdef DEBUG_SCHEDULER
#define schedbg(m, ...) debug(m, ##__VA_ARGS__)
#else
#define schedbg(m, ...)
#endif

// #define DEBUG_TASK_MANAGER 1

#ifdef DEBUG_TASK_MANAGER
#include <display.h>
#endif

namespace Tasking
{
    bool MultitaskingSchedulerEnabled = false;
    Multitasking *mt = nullptr;

#ifdef DEBUG_TASK_MANAGER
    void drawrectangle(uint64_t X, uint64_t Y, uint64_t W, uint64_t H, uint32_t C)
    {
        for (int y = Y; y < Y + H; y++)
            for (int x = X; x < X + W; x++)
                CurrentDisplay->SetPixel(x, y, C);
    }

    void TraceSchedOnScreen()
    {
        CurrentDisplay->ResetPrintPosition();
        drawrectangle(0, 0, 222, CurrentDisplay->GetFramebuffer()->Height / 2, 0x282828);
        bool showarrow = false;

        CurrentDisplay->SetPrintColor(0xF222F2);
        if (mt->CurrentProcess == mt->IdleProcess)
            printf("Idle Process Running\n");
        if (mt->CurrentThread == mt->IdleThread)
            printf("Idle Thread Running\n");

        foreach (auto Proc1 in mt->ListProcess)
        {
            showarrow = false;
            CurrentDisplay->SetPrintColor(0xFF2200);
            if (Proc1 == mt->CurrentProcess)
                showarrow = true;
            printf("%s(%d) %s [%d%%/%lld]\n", Proc1->Name, Proc1->ID, showarrow ? "<-" : "  ", Proc1->Info.Usage[0], Proc1->Info.UsedTicks);
            foreach (auto thd in Proc1->Threads)
            {
                showarrow = false;
                CurrentDisplay->SetPrintColor(0x00FF22);
                if (thd == mt->CurrentThread)
                    showarrow = true;
                printf(" \\%s(%d) %s [%d%%/%lld]\n", Proc1->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
            }
            foreach (auto Proc1Children in Proc1->Children)
            {
                showarrow = false;
                CurrentDisplay->SetPrintColor(0xFF2200);
                if (Proc1Children == mt->CurrentProcess)
                    showarrow = true;
                printf(" \\%s(%d) %s [%d%%/%lld]\n", Proc1Children->Name, Proc1Children->ID, showarrow ? "<-" : "  ", Proc1Children->Info.Usage[0], Proc1Children->Info.UsedTicks);
                foreach (auto thd in Proc1Children->Threads)
                {
                    showarrow = false;
                    CurrentDisplay->SetPrintColor(0x00FF22);
                    if (thd == mt->CurrentThread)
                        showarrow = true;
                    printf("  \\%s(%d) %s [%d%%/%lld]\n", Proc1Children->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
                }
                foreach (auto Proc2Children in Proc1Children->Children)
                {
                    showarrow = false;
                    CurrentDisplay->SetPrintColor(0xFF2200);
                    if (Proc2Children == mt->CurrentProcess)
                        showarrow = true;
                    printf("  \\%s(%d) %s [%d%%/%lld]\n", Proc2Children->Name, Proc2Children->ID, showarrow ? "<-" : "  ", Proc2Children->Info.Usage[0], Proc2Children->Info.UsedTicks);
                    foreach (auto thd in Proc2Children->Threads)
                    {
                        showarrow = false;
                        CurrentDisplay->SetPrintColor(0x00FF22);
                        if (thd == mt->CurrentThread)
                            showarrow = true;
                        printf("   \\%s(%d) %s [%d%%/%lld]\n", Proc2Children->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
                    }
                    foreach (auto Proc3Children in Proc2Children->Children)
                    {
                        showarrow = false;
                        CurrentDisplay->SetPrintColor(0xFF2200);
                        if (Proc3Children == mt->CurrentProcess)
                            showarrow = true;
                        printf("   \\%s(%d) %s [%d%%/%lld]\n", Proc3Children->Name, Proc3Children->ID, showarrow ? "<-" : "  ", Proc3Children->Info.Usage[0], Proc3Children->Info.UsedTicks);
                        foreach (auto thd in Proc3Children->Threads)
                        {
                            showarrow = false;
                            CurrentDisplay->SetPrintColor(0x00FF22);
                            if (thd == mt->CurrentThread)
                                showarrow = true;
                            printf("     \\%s(%d) %s [%d%%/%lld]\n", Proc3Children->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
                        }
                        foreach (auto Proc4Children in Proc3Children->Children)
                        {
                            showarrow = false;
                            CurrentDisplay->SetPrintColor(0xFF2200);
                            if (Proc4Children == mt->CurrentProcess)
                                showarrow = true;
                            printf("     \\%s(%d) %s [%d%%/%lld]\n", Proc4Children->Name, Proc4Children->ID, showarrow ? "<-" : "  ", Proc4Children->Info.Usage[0], Proc4Children->Info.UsedTicks);
                            foreach (auto thd in Proc4Children->Threads)
                            {
                                showarrow = false;
                                CurrentDisplay->SetPrintColor(0x00FF22);
                                if (thd == mt->CurrentThread)
                                    showarrow = true;
                                printf("      \\%s(%d) %s [%d%%/%lld]\n", Proc4Children->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
                            }
                            foreach (auto Proc5Children in Proc4Children->Children)
                            {
                                showarrow = false;
                                CurrentDisplay->SetPrintColor(0xFF2200);
                                if (Proc5Children == mt->CurrentProcess)
                                    showarrow = true;
                                printf("      \\%s(%d) %s [%d%%/%lld]\n", Proc5Children->Name, Proc5Children->ID, showarrow ? "<-" : "  ", Proc5Children->Info.Usage[0], Proc5Children->Info.UsedTicks);
                                foreach (auto thd in Proc5Children->Threads)
                                {
                                    showarrow = false;
                                    CurrentDisplay->SetPrintColor(0x00FF22);
                                    if (thd == mt->CurrentThread)
                                        showarrow = true;
                                    printf("       \\%s(%d) %s [%d%%/%lld]\n", Proc5Children->Name, thd->ID, showarrow ? "<-" : "  ", thd->Info.Usage[0], thd->Info.UsedTicks);
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    }
#endif

    uint64_t CreateToken()
    {
        // https://wiki.osdev.org/Random_Number_Generator
        // TODO: The kernel should have a way to remember what tokens are generated.
        return rand64();
    }

    extern "C" void ProcessDoExit(uint64_t Code)
    {
        EnterCriticalSection;
        mt->CurrentThread->Status = STATUS::Terminated;
        mt->CurrentThread->ExitCode = Code;
        schedbg("parent:%s tid:%d, code:%016p", mt->CurrentProcess->Name, mt->CurrentThread->ID, Code);
        trace("Exiting thread %d(%s)...", mt->CurrentThread->ID, mt->CurrentThread->Name);
        LeaveCriticalSection;
        apic->OneShot(SchedulerInterrupt, 100);
        CPU_STOP;
    }

    void UpdateTimeUsed(GeneralProcessInfo *Info)
    {
        uint64_t CurrentCount = counter();
        if (Info->LastUsedTicks == 0)
        {
            Info->UsedTicks += CurrentCount - Info->SpawnTick;
            Info->LastUsedTicks = CurrentCount;
        }
        else
        {
            Info->UsedTicks += CurrentCount - Info->LastUsedTicks;
            Info->LastUsedTicks = CurrentCount;
        }
    }

    void UpdateCPUUsage(GeneralProcessInfo *Info)
    {
        // uint64_t CurrentCount = counter();
        // TODO: for Info->Usage[cpu_count] = ...
        // for (size_t i = 0; i < 2; i++)
        // {
        // Info->Usage[i] = (uint32_t)((CurrentCount - Info->LastUsedTicks) * 100 / (CurrentCount - Info->SpawnTick));

        // if (Info->Usage[i] > 100)
        // Info->Usage[i] = 100;
        // }
    }

    void SetInfo(GeneralProcessInfo *Info)
    {
        Info->SpawnTick = counter();

        uint32_t t = 0;
        outb(0x70, 0x00);
        t = inb(0x71);
        Info->Second = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x02);
        t = inb(0x71);
        Info->Minute = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x04);
        t = inb(0x71);
        Info->Hour = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x07);
        t = inb(0x71);
        Info->Day = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x08);
        t = inb(0x71);
        Info->Month = ((t & 0x0F) + ((t >> 4) * 10));
        outb(0x70, 0x09);
        t = inb(0x71);
        Info->Year = ((t & 0x0F) + ((t >> 4) * 10));
    }

    PCB *Multitasking::CreateProcess(PCB *Parent, char *Name, ELEVATION Elevation, int Priority)
    {
        EnterCriticalSection;
        PCB *process = new PCB;
        process->Checksum = Checksum::PROCESS_CHECKSUM;

        SetInfo(&process->Info);
        process->Info.Priority = Priority;
        process->Security.Token = CreateToken();
        trace("New security token created %p", process->Security.Token);
        process->ID = this->NextPID++;
        process->Elevation = Elevation;
        process->Status = STATUS::Ready;
        memcpy(process->Name, Name, sizeof(process->Name));
        if (Parent)
        {
            process->Parent = Parent;
            process->Parent->Children.push_back(process);
        }
        CR3 cr3;
        if (Elevation == ELEVATION::User)
            cr3.raw = (uint64_t)KernelPageTableAllocator->CreatePageTable(true);
        else
            cr3.raw = (uint64_t)KernelPageTableAllocator->CreatePageTable(false);
        process->PageTable = cr3;
        schedbg("Address space for %s has been created at %#llx", Name, process->PageTable.raw);
        ListProcess.push_back(process);
        trace("New process %s (%d) created.", process->Name, process->ID);
        LeaveCriticalSection;
        return process;
    }

    TCB *Multitasking::CreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t Arg0, uint64_t Arg1, int Priority)
    {
        EnterCriticalSection;
        if (Parent == nullptr || Parent->Checksum != Checksum::PROCESS_CHECKSUM)
        {
            err("Null parent process not allowed.");
            LeaveCriticalSection;
            return nullptr;
        }
        TCB *thread = new TCB;
        thread->Checksum = Checksum::THREAD_CHECKSUM;

        thread->ID = this->NextTID++;
        thread->Status = STATUS::Ready;
        thread->Security = Parent->Security;
        thread->Parent = Parent;
        memcpy(thread->Name, Parent->Name, sizeof(Parent->Name));
        memset(&thread->Registers, 0, sizeof(REGISTERS));

        schedbg("Parent elevation is %d", Parent->Elevation);

        switch (Parent->Elevation)
        {
        case ELEVATION::System:
            err("Elevation not supported. Using kernel elevation.");
        case ELEVATION::Idle:
        case ELEVATION::Kernel:
            thread->Registers.ds = GDT_KERNEL_DATA;
            thread->Registers.cs = GDT_KERNEL_CODE;
            thread->Registers.ss = GDT_KERNEL_DATA;
            thread->gs = (uint64_t)thread;
            thread->fs = 0;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
            thread->Stack = KernelStackAllocator->AllocateStack();
            thread->Registers.STACK = (uint64_t)thread->Stack;
            POKE(uint64_t, thread->Registers.rsp) = (uint64_t)ProcessDoExit;
            break;
        case ELEVATION::User:
            thread->Registers.ds = GDT_USER_DATA;
            thread->Registers.cs = GDT_USER_CODE;
            thread->Registers.ss = GDT_USER_DATA;
            thread->gs = 0;
            thread->fs = 0;
            thread->Registers.rflags.always_one = 1;
            thread->Registers.rflags.IF = 1;
            thread->Registers.rflags.ID = 1;
            thread->Stack = KernelStackAllocator->AllocateStack(true);
            thread->Registers.STACK = (uint64_t)thread->Stack;
            /*
                We can't add ProcessDoExit at the end of the stack because it will
                trigger a page fault or invalid general protection fault exception (because
                it will execute kernel code ofc).
                We need to leave the libc's crt to make a syscall when the thread is exited.
            */
            break;
        default:
            err("Unknown elevation.");
            delete thread;
            LeaveCriticalSection;
            return nullptr;
        }

        thread->Registers.FUNCTION = (uint64_t)InstructionPointer;
        thread->Registers.ARG0 = (uint64_t)Arg0; // args0
        thread->Registers.ARG1 = (uint64_t)Arg1; // args1

        SetInfo(&thread->Info);
        thread->Info.Priority = Priority;

        thread->Msg = (MessageQueue *)KernelAllocator.RequestPages(2);
        KernelPageTableManager.MapMemory(thread->Msg, thread->Msg, PTFlag::US | PTFlag::RW);
        KernelPageTableManager.MapMemory(thread->Msg + PAGE_SIZE, thread->Msg + PAGE_SIZE, PTFlag::US | PTFlag::RW);

        Parent->Threads.push_back(thread);
        trace("New thread %d created (%s).", thread->ID, thread->Name);
        LeaveCriticalSection;
        return thread;
    }

    inline bool InvalidPCB(PCB *pcb)
    {
        if (pcb == nullptr)
        {
            // schedbg("Invalid PCB.");
            return true;
        }
        else if (pcb->Checksum != Checksum::PROCESS_CHECKSUM)
        {
            // schedbg("Invalid PCB checksum.");
            return true;
        }
        else if (pcb->Elevation == ELEVATION::Idle)
        {
            // schedbg("Invalid PCB (Idle process).");
            return true;
        }
        return false;
    }

    inline bool InvalidTCB(TCB *tcb)
    {
        if (tcb == nullptr)
        {
            // schedbg("Invalid TCB.");
            return true;
        }
        else if (tcb->Checksum != Checksum::THREAD_CHECKSUM)
        {
            // schedbg("Invalid TCB checksum.");
            return true;
        }
        else if (tcb->Parent->Elevation == ELEVATION::Idle)
        {
            // schedbg("Invalid TCB (Child of Idle process).");
            return true;
        }
        return false;
    }

    void RemoveThread(TCB *tcb)
    {
        for (uint64_t i = 0; i < tcb->Parent->Threads.size(); i++)
            if (tcb->Parent->Threads[i] == tcb)
            {
                trace("Thread %d terminated", tcb->ID);
                KernelStackAllocator->FreeStack(tcb->Parent->Threads[i]->Stack);
                tcb->Parent->Threads[i]->Checksum = badfennec;
                kfree(tcb->Parent->Threads[i]);
                tcb->Parent->Threads.remove(i);
                break;
            }
    }

    void RemoveProcess(PCB *pcb)
    {
        if (pcb == nullptr)
            return;

        if (pcb->Status == Terminated)
        {
            foreach (TCB *thread in pcb->Threads)
            {
                RemoveThread(thread);
            }

            foreach (PCB *process in pcb->Children)
            {
                RemoveProcess(process);
            }

            for (size_t i = 0; i < mt->ListProcess.size(); i++)
            {
                if (mt->ListProcess[i] == pcb)
                {
                    trace("pcb %d terminated", mt->ListProcess[i]->ID);
                    KernelPageTableAllocator->RemovePageTable(reinterpret_cast<VMM::PageTable *>(mt->ListProcess[i]->PageTable.raw));
                    mt->ListProcess[i]->Checksum = badfennec;
                    kfree(mt->ListProcess[i]);
                    mt->ListProcess.remove(i);
                    break;
                }
            }
        }
        else
        {
            foreach (TCB *thread in pcb->Threads)
            {
                if (thread->Status == Terminated)
                    RemoveThread(thread);
            }
        }
    }

    void UpdatePageTable(CR3 pt)
    {
        if (pt.raw == 0)
            return;
        if (pt.raw != readcr3().raw)
            writecr3(pt);
    }

    Critical::CriticalSectionData *CriticalSectionData = nullptr;

    extern "C"
    {
        __attribute__((naked, used)) void MultiTaskingSchedulerHelper()
        {
            asm("cld\n"
                "pushq %rax\n"
                "pushq %rbx\n"
                "pushq %rcx\n"
                "pushq %rdx\n"
                "pushq %rsi\n"
                "pushq %rdi\n"
                "pushq %rbp\n"
                "pushq %r8\n"
                "pushq %r9\n"
                "pushq %r10\n"
                "pushq %r11\n"
                "pushq %r12\n"
                "pushq %r13\n"
                "pushq %r14\n"
                "pushq %r15\n"
                "movq %ds, %rax\n"
                "pushq %rax\n"
                "movw $16, %ax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "movw %ax, %ss\n"
                "movq %rsp, %rdi\n"
                "call MultiTaskingSchedulerHandler\n"
                "popq %rax\n"
                "movw %ax, %ds\n"
                "movw %ax, %es\n"
                "popq %r15\n"
                "popq %r14\n"
                "popq %r13\n"
                "popq %r12\n"
                "popq %r11\n"
                "popq %r10\n"
                "popq %r9\n"
                "popq %r8\n"
                "popq %rbp\n"
                "popq %rdi\n"
                "popq %rsi\n"
                "popq %rdx\n"
                "popq %rcx\n"
                "popq %rbx\n"
                "popq %rax\n"
                "addq $16, %rsp\n"
                "iretq");
        }

        static void MakeOneShot() { apic->OneShot(SchedulerInterrupt, 100); }

        __attribute__((naked, used)) static void IdleProcessLoop()
        {
            asm volatile("idleloop:\n"
                         "call MakeOneShot\n"
                         "hlt\n"
                         "jmp idleloop\n");
        }

        InterruptHandler(MultiTaskingSchedulerHandler)
        {
            if (!MultitaskingSchedulerEnabled)
            {
                apic->OneShot(SchedulerInterrupt, 100);
                EndOfInterrupt(INT_NUM); // apic->IPI(0, SchedulerInterrupt);
                return;
            }
            CriticalSectionData->EnableInterrupts = InterruptsEnabled();
            CLI;
            LOCK(CriticalSectionData->CriticalLock);

#ifdef DEBUG_TASK_MANAGER
            static int slowschedule = 0;
            if (slowschedule < 400)
            {
                slowschedule++;
                UNLOCK(CriticalSectionData->CriticalLock);
                if (CriticalSectionData->EnableInterrupts)
                    STI;
                apic->OneShot(SchedulerInterrupt, 200);
                EndOfInterrupt(INT_NUM); // apic->IPI(0, SchedulerInterrupt);
                return;
            }
            else
                slowschedule = 0;
            TraceSchedOnScreen();
#endif
            schedbg("Status: 0-ukn | 1-rdy | 2-run | 3-wait | 4-term");
            // Null or invalid process/thread? Let's find a new one to execute.
            if (InvalidPCB(mt->CurrentProcess) || InvalidTCB(mt->CurrentThread))
            {
                schedbg("%d processes", mt->ListProcess.size());
#ifdef DEBUG_SCHEDULER
                foreach (auto var in mt->ListProcess)
                {
                    schedbg("Process %d %s", var->ID, var->Name);
                }
#endif
                // Find a new process to execute.
                foreach (PCB *pcb in mt->ListProcess)
                {
                    if (InvalidPCB(pcb))
                        continue;

                    // Check process status.
                    switch (pcb->Status)
                    {
                    case STATUS::Ready:
                        schedbg("Ready process (%s)%d", pcb->Name, pcb->ID);
                        break;
                    default:
                        schedbg("Process %s(%d) status %d", pcb->Name, pcb->ID, pcb->Status);
                        RemoveProcess(pcb);
                        continue;
                    }

                    // Get first available thread from the list.
                    foreach (TCB *tcb in pcb->Threads)
                    {
                        if (InvalidTCB(tcb))
                            continue;

                        if (tcb->Status != STATUS::Ready)
                            continue;

                        // Set process and thread as the current one's.
                        mt->CurrentProcess = pcb;
                        mt->CurrentThread = tcb;
                        // Success!
                        goto Success;
                    }
                }
                schedbg("No process to run.");
                // No process found. Idling...
                goto Idle;
            }
            else
            {
                // Save current process and thread registries, gs, fs, fpu, etc...
                mt->CurrentThread->Registers = *regs;
                mt->CurrentThread->gs = rdmsr(MSR_SHADOW_GS_BASE);
                mt->CurrentThread->fs = rdmsr(MSR_FS_BASE);
                fxsave(mt->CurrentThread->FXRegion);

                // Set the process & thread as ready if it's running.
                if (mt->CurrentProcess->Status == STATUS::Running)
                    mt->CurrentProcess->Status = STATUS::Ready;
                if (mt->CurrentThread->Status == STATUS::Running)
                    mt->CurrentThread->Status = STATUS::Ready;

                // Get next available thread from the list.
                for (uint64_t i = 0; i < mt->CurrentProcess->Threads.size(); i++)
                {
                    // Loop until we find the current thread from the process thread list.
                    if (mt->CurrentProcess->Threads[i] != mt->CurrentThread)
                        continue;

                    // Check if the next thread is valid. If not, we search until we find, but if we reach the end of the list, we go to the next process.
                    TCB *thread = mt->CurrentProcess->Threads[i + 1];
                    if (InvalidTCB(thread))
                        continue;

                    schedbg("%s(%d) and next thread is %s(%d)", mt->CurrentProcess->Threads[i]->Name, mt->CurrentProcess->Threads[i]->ID, thread->Name, thread->ID);

                    // Check if the process is ready to be executed.
                    if (mt->CurrentProcess->Status != STATUS::Ready)
                    {
                        schedbg("Process %s is not ready", mt->CurrentProcess->Name);
                        break;
                    }

                    // Check if the thread is ready to be executed.
                    if (thread->Status != STATUS::Ready)
                    {
                        schedbg("Thread %d is not ready", thread->ID);
                        continue;
                    }
                    // Everything is fine, we can set the new thread as the current one.
                    mt->CurrentThread = thread;
                    schedbg("[thd 0 -> end] Scheduling thread %d parent of %s->%d Procs %d", thread->ID, thread->Parent->Name, mt->CurrentProcess->Threads.size(), mt->ListProcess.size());
                    // Yay! We found a new thread to execute.
                    goto Success;
                }

                // If the last process didn't find a thread to execute, we search for a new process.
                for (uint64_t i = 0; i < mt->ListProcess.size(); i++)
                {
                    // Loop until we find the current process from the process list.
                    if (mt->ListProcess[i] == mt->CurrentProcess)
                    {
                        // Check if the next process is valid. If not, we search until we find.
                        PCB *pcb = mt->ListProcess[i + 1];

                        if (InvalidPCB(pcb))
                            continue;
                        if (pcb->Status != STATUS::Ready)
                            continue;

                        // Everything good, now search for a thread.
                        for (uint64_t j = 0; j < pcb->Threads.size(); j++)
                        {
                            TCB *tcb = pcb->Threads[j];
                            if (InvalidTCB(tcb))
                                continue;
                            if (tcb->Status != STATUS::Ready)
                                continue;
                            // Success! We set as the current one and restore the stuff.
                            mt->CurrentProcess = pcb;
                            mt->CurrentThread = tcb;
                            schedbg("[cur proc+1 -> first thd] Scheduling thread %d %s->%d (Total Procs %d)", tcb->ID, tcb->Name, pcb->Threads.size(), mt->ListProcess.size());
                            goto Success;
                        }
                    }
                }

                // Before checking from the beginning, we remove everything that is terminated.
                foreach (PCB *pcb in mt->ListProcess)
                {
                    if (InvalidPCB(pcb))
                        continue;
                    // RemoveProcess(pcb); // comment this until i will find a way to handle properly vectors, the memory need to be 0ed after removing.
                }

                // If we didn't find anything, we check from the start of the list. This is the last chance to find something or we go to idle.
                foreach (PCB *pcb in mt->ListProcess)
                {
                    if (InvalidPCB(pcb))
                        continue;
                    if (pcb->Status != STATUS::Ready)
                        continue;

                    // Now do the thread search!
                    foreach (TCB *tcb in pcb->Threads)
                    {
                        if (InvalidTCB(tcb))
                            continue;
                        if (tcb->Status != STATUS::Ready)
                            continue;
                        // \o/ We found a new thread to execute.
                        mt->CurrentProcess = pcb;
                        mt->CurrentThread = tcb;
                        schedbg("[proc 0 -> end -> first thd] Scheduling thread %d parent of %s->%d (Procs %d)", tcb->ID, tcb->Parent->Name, pcb->Threads.size(), mt->ListProcess.size());
                        goto Success;
                    }
                }
            }

        Idle:
        {
            // I should remove processes that are no longer having any threads? remove only from userspace?
            if (mt->IdleProcess == nullptr)
            {
                schedbg("Idle process created");
                mt->IdleProcess = mt->CreateProcess(nullptr, (char *)"idle", ELEVATION::Idle);
                mt->IdleThread = mt->CreateThread(mt->IdleProcess, reinterpret_cast<uint64_t>(IdleProcessLoop), 0, 0);
            }
            mt->CurrentProcess = mt->IdleProcess;
            mt->CurrentThread = mt->IdleThread;

            *regs = mt->CurrentThread->Registers;
            CR3 cr3;
            cr3.raw = (uint64_t)KernelPageTableManager.PML4;
            UpdatePageTable(cr3);

            wrmsr(MSR_FS_BASE, mt->CurrentThread->fs);
            wrmsr(MSR_GS_BASE, (uint64_t)mt->CurrentThread);
            wrmsr(MSR_SHADOW_GS_BASE, (uint64_t)mt->CurrentThread);
            fxrstor(mt->CurrentThread->FXRegion);
            goto End;
        }

        Success:
        {
            schedbg("Success Prc:%s(%d) Thd:%s(%d)",
                    mt->CurrentProcess->Name, mt->CurrentProcess->ID,
                    mt->CurrentThread->Name, mt->CurrentThread->ID);
            mt->CurrentProcess->Status = STATUS::Running;
            mt->CurrentThread->Status = STATUS::Running;

            *regs = mt->CurrentThread->Registers;
            // UpdatePageTable(mt->CurrentProcess->PageTable); // the page table issue... i don't know how to fix it.

            wrmsr(MSR_FS_BASE, mt->CurrentThread->fs);
            wrmsr(MSR_GS_BASE, (uint64_t)mt->CurrentThread);
            switch (mt->CurrentProcess->Elevation)
            {
            case ELEVATION::System:
            case ELEVATION::Idle:
            case ELEVATION::Kernel:
                wrmsr(MSR_SHADOW_GS_BASE, (uint64_t)mt->CurrentThread);
                break;
            case ELEVATION::User:
                wrmsr(MSR_SHADOW_GS_BASE, mt->CurrentThread->gs);
                break;
            default:
                err("Unknown elevation.");
                break;
            }
            fxrstor(mt->CurrentThread->FXRegion);
        }
        End:
        {
            UpdateTimeUsed(&mt->CurrentProcess->Info);
            UpdateTimeUsed(&mt->CurrentThread->Info);
            UpdateCPUUsage(&mt->CurrentProcess->Info);
            UpdateCPUUsage(&mt->CurrentThread->Info);
            UNLOCK(CriticalSectionData->CriticalLock);
            if (CriticalSectionData->EnableInterrupts)
                STI;
            apic->OneShot(SchedulerInterrupt, mt->CurrentThread->Info.Priority);
            EndOfInterrupt(INT_NUM); // apic->IPI(0, SchedulerInterrupt);
        }
        }
    }

    Multitasking::Multitasking()
    {
        CurrentTaskingMode = TaskingMode::Multi;
        CriticalSectionData = new Critical::CriticalSectionData;
        apic->RedirectIRQ(0, SchedulerInterrupt - 32, 1);
        apic->OneShot(SchedulerInterrupt, 100);
    }

    Multitasking::~Multitasking()
    {
        CurrentTaskingMode = TaskingMode::None;
        delete CriticalSectionData;
    }
}
