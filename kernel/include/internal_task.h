#include <types.h>
#include <interrupts.h>
#include <heap.h>
#include <msg.h>
#ifdef __cplusplus
#include <vector.hpp>
#endif
#include <task.h>

#ifdef __cplusplus

enum TaskingMode
{
    None,
    Mono,
    Multi,
    MultiV2
};

extern int CurrentTaskingMode;

namespace MonoTasking
{
    enum TaskState
    {
        TaskStateNull,
        TaskStateRunning,
        TaskStateWaiting,
        TaskStateTerminated,
    };

#define TASK_CHECKSUM 0xDEADCAFE
    struct TaskControlBlock
    {
        char name[256];
        REGISTERS regs;
        uint64_t id;
        void *stack;
        void *pml4;
        bool UserMode;
        enum TaskState state;
        unsigned int checksum;
    };

#define MAX_TASKS 0x10000

    static TaskControlBlock *CurrentTask = nullptr;
    static TaskControlBlock *TaskQueue[MAX_TASKS];

    class MonoTasking
    {
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
        TaskControlBlock *CreateTask(uint64_t InstructionPointer, uint64_t FirstArgument, uint64_t SecondArgument, char *Name, bool UserMode);

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

    private:
    };

    extern MonoTasking *SingleProcessing;
};

namespace MultiTasking
{
    enum Checksum
    {
        PROCESS_CHECKSUM = 0xCAFEBABE,
        THREAD_CHECKSUM = 0xDEADCAFE
    };

#define check_process(process)                 \
    if (process == nullptr)                    \
        continue;                              \
    if (process->Checksum != PROCESS_CHECKSUM) \
        continue;

#define check_thread(thread)                 \
    if (thread == nullptr)                   \
        continue;                            \
    if (thread->Checksum != THREAD_CHECKSUM) \
        continue;

    static ProcessControlBlock *CurrentProcess = nullptr;
    static ThreadControlBlock *CurrentThread = nullptr;
    static ProcessControlBlock *IdleProcess = nullptr;
    static ThreadControlBlock *IdleThread = nullptr;

    class MultiTasking
    {
    public:
        uint64_t NextPID = 0, NextTID = 0;

        ProcessControlBlock *GetCurrentProcess();
        ThreadControlBlock *GetCurrentThread();
        Vector<ProcessControlBlock *> GetVectorProcessList();

        ProcessControlBlock *CreateProcess(ProcessControlBlock *parent, char *name);
        ThreadControlBlock *CreateThread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy, bool UserMode);

        void Schedule();
        void ToggleScheduler(bool toggle);
        /**
         * @brief Construct a new Multi Tasking object
         *
         */
        MultiTasking();
        /**
         * @brief Destroy the Multi Tasking object
         *
         */
        ~MultiTasking();

    private:
    };
    extern MultiTasking *MultiProcessing;
};

namespace MultiTaskingV2
{
    // TODO: For future use. For now it's not important.
    class MultiTaskingV2
    {
    private:
        enum Checksum
        {
            PROCESS_CHECKSUM = 0xCAFEBABE,
            THREAD_CHECKSUM = 0xDEADCAFE
        };

    public:
        uint64_t NextPID = 0, NextTID = 0, NextIndexToSchedule = 0;
        ProcessControlBlock *CurrentProcess;
        ThreadControlBlock *GetCurrentThread;
        Vector<ThreadControlBlock *> ListProcess;

        ProcessControlBlock *CreateProcess(ProcessControlBlock *Parent, char *Name);
        ThreadControlBlock *CreateThread(ProcessControlBlock *Parent,
                                         uint64_t InstructionPointer,
                                         uint64_t Arg0, uint64_t Arg1,
                                         enum ControlBlockPolicy Policy = POLICY_KERNEL,
                                         enum ControlBlockPriority Priority = PRIORITY_MEDIUM,
                                         enum ControlBlockState State = STATE_READY);

        void Schedule();
        /**
         * @brief Construct a new Multi Tasking object
         *
         */
        MultiTaskingV2();
        /**
         * @brief Destroy the Multi Tasking object
         *
         */
        ~MultiTaskingV2();
    };
}

#endif

START_EXTERNC

/**
 * @brief Get a running process by PID
 *
 * @param pid Process identifier
 * @return ProcessControlBlock
 */
ProcessControlBlock *FENAPI SysGetProcessByPID(uint64_t ID);

/**
 * @brief Get a running thread by PID
 *
 * @param pid Thread identifier
 * @return ThreadControlBlock
 */
ThreadControlBlock *FENAPI SysGetThreadByTID(uint64_t ID);

/**
 * @brief Get current process
 *
 * @return ProcessControlBlock
 */
ProcessControlBlock *FENAPI SysGetCurrentProcess();

/**
 * @brief Get current thread
 *
 * @return ThreadControlBlock
 */
ThreadControlBlock *FENAPI SysGetCurrentThread();

/**
 * @brief Create a new process from a file
 *
 * @param File TODO: more
 * @return ProcessControlBlock
 */
ProcessControlBlock *FENAPI SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, bool usermode);

/**
 * @brief Create a new simple process with custom name and address space
 *
 * @param Name
 * @param PageTable
 * @return ProcessControlBlock*
 */
ProcessControlBlock *FENAPI SysCreateProcess(const char *Name, void *PageTable);

/**
 * @brief Create a new thread
 *
 * @param Parent if null is using the current running
 * @param InstructionPointer
 * @return ThreadControlBlock*
 */
ThreadControlBlock *FENAPI SysCreateThread(ProcessControlBlock *Parent, uint64_t InstructionPointer, uint64_t arg0, uint64_t arg1, bool UserMode);

void do_exit(uint64_t code);
void schedule();
void StartTasking(uint64_t Address, enum TaskingMode Mode);

void init_syscalls();

END_EXTERNC
