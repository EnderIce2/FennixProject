#pragma once
#include <interrupts.h>
#include <types.h>
#include <heap.h>
#include <task.h>

enum TaskingMode
{
    None,
    Mono,
    Multi
};

extern int CurrentTaskingMode;

#ifdef __cplusplus

namespace Tasking
{
    enum TaskState
    {
        TaskStateNull,
        TaskStateReady,
        TaskStateRunning,
        TaskStateWaiting,
        TaskStateTerminated,
        TaskPushed,
        TaskPoped,
    };

#define TASK_CHECKSUM 0xDEADCAFE
    struct TaskControlBlock
    {
        char name[256];
        TrapFrame regs;
        uint64_t fs, gs;
        uint64_t id;
        void *stack;
        VMM::PageTable *pml4;
        bool UserMode;
        enum TaskState state;
        int argc;
        char **argv;
        char **envp;
        uint64_t SpawnTick;
        uint64_t Year, Month, Day, Hour, Minute, Second;
        unsigned int checksum;
    };

    class MonoTasking
    {
    private:
        void SetTaskTimeInfo(TaskControlBlock *task);

    public:
        /**
         * @brief Create a new Task
         *
         * @param rip Instruction Pointer
         * @param args0 First argument
         * @param args1 Second argument
         * @param name The name of the new Task
         * @return The new created Task
         */
        TaskControlBlock *CreateTask(uint64_t InstructionPointer, uint64_t FirstArgument, uint64_t SecondArgument, char *Name, bool UserMode, bool AfterCurrent = true);

        /**
         * @brief Kill the current Task
         */
        void KillMe();

        /**
         * @brief Move to the next Task and suspending the current
         */
        void PushTask(uint64_t rip);

        /**
         * @brief Move to the previous Task and suspending the current
         */
        void PopTask(bool Destroy = false);

        /**
         * @brief Get the current Task
         *
         * @return The current Task
         */
        TaskControlBlock *GetCurrentTask();

        /**
         * @brief Construct a new Mono Tasking object
         *
         * @param firstThread The first Instruction Pointer to be executed
         */
        MonoTasking(uint64_t FirstTask);

        /**
         * @brief Destroy the Mono Tasking object (Make sure that all other processes are destroyed and no stack is used!)
         *
         */
        ~MonoTasking();
    };

    class Multitasking
    {
    private:
    public:
        Vector<PCB *> ListProcess;
        uint64_t NextPID = 0, NextTID = 0;

        PCB *IdleProcess = nullptr;
        TCB *IdleThread = nullptr;

        PCB *CreateProcess(PCB *Parent, char *Name, CBElevation Elevation, int Priority = 1);
        TCB *CreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t Arg0, uint64_t Arg1,
                          int Priority = 1, enum Architecture Architecture = Architecture::x64, enum Platform Platform = Platform::Native);

        Multitasking();
        ~Multitasking();
    };

    extern bool MultitaskingSchedulerEnabled;
    extern MonoTasking *monot;
    extern Multitasking *mt;
}

#endif

START_EXTERNC

PCB *ConvertTaskCBToPCB(Tasking::TaskControlBlock *task);
TCB *ConvertTaskCBToTCB(Tasking::TaskControlBlock *task);

/**
 * @brief Get a running process by PID
 *
 * @param pid Process identifier
 * @return PCB
 */
PCB *SysGetProcessByPID(uint64_t ID);

/**
 * @brief Get a running thread by PID
 *
 * @param pid Thread identifier
 * @return TCB
 */
TCB *SysGetThreadByTID(uint64_t ID);

/**
 * @brief Get current process
 *
 * @return PCB
 */
PCB *SysGetCurrentProcess();

/**
 * @brief Get current thread
 *
 * @return TCB
 */
TCB *SysGetCurrentThread();

/**
 * @brief Set current process priority
 *
 * @param Priority
 */
void SysSetProcessPriority(int Priority);

/**
 * @brief Get current process priority
 *
 * @return int
 */
int SysGetProcessPriority();

/**
 * @brief Get current thread priority
 *
 * @param Priority
 */
void SysSetThreadPriority(int Priority);

/**
 * @brief Get current thread priority
 *
 * @return int
 */
int SysGetThreadPriority();

/**
 * @brief Create a new process from a file
 *
 * @param File TODO: more
 * @return PCB
 */
PCB *SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, CBElevation Elevation);

/**
 * @brief Create a new simple process with custom name and address space
 *
 * @param Name
 * @param PageTable
 * @return PCB*
 */
PCB *SysCreateProcess(const char *Name, CBElevation Elevation);

/**
 * @brief Create a new thread
 *
 * @param Parent if null is using the current running
 * @param InstructionPointer
 * @return TCB*
 */
TCB *SysCreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t arg0, uint64_t arg1);

void StartTasking(uint64_t Address, enum TaskingMode Mode);

void init_syscalls();

END_EXTERNC
