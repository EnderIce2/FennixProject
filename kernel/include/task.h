#include <types.h>
#include <interrupts.h>
#include <heap.h>

/**
 * @brief This is kernel calls for process creation and basic operations.
 *
 */

#define MAX_PROCESSES 0x10
#define PROCESS_MAX_THREADS 0x10

enum ControlBlockState
{
    STATE_UNKNOWN,
    STATE_READY,
    STATE_RUNNING,
    STATE_WAITING,
    STATE_TERMINATED,
};

enum ControlBlockPolicy
{
    POLICY_UNKNOWN,
    POLICY_KERNEL,
    POLICY_SYSTEM,
    POLICY_USER,
};

enum ControlBlockPriority
{
    PRIORITY_UNKNOWN,
    PRIORITY_REALTIME,
    PRIORITY_VERYHIGH,
    PRIORITY_HIGH,
    PRIORITY_MEDIUM,
    PRIORITY_LOW,
    PRIORITY_VERYLOW,
    /* ... */
    PRIORITY_IDLE = 100,
};

typedef struct _ControlBlockTime
{
    uint64_t ticks_used;
    uint64_t tick;
    uint64_t y;
    uint64_t M;
    uint64_t d;
    uint64_t h;
    uint64_t m;
    uint64_t s;
} ControlBlockTime;

typedef struct _ThreadControlBlock
{
    uint64_t ThreadID;
    enum ControlBlockState State;
    enum ControlBlockPolicy Policy;
    enum ControlBlockPriority Priority;
    struct _ProcessControlBlock *Parent;
    void *Stack;
    REGISTERS Registers;
    uint64_t ExitCode;
    ControlBlockTime *Time;
    uint32_t Checksum;
} ThreadControlBlock;

typedef struct _ProcessControlBlock
{
    uint64_t ProcessID;
    char Name[256];
    enum ControlBlockState State;
    struct _ProcessControlBlock *Parent;
    uint64_t ExitCode;
#ifndef __cplusplus
    void *PageTable;
#else
    VMM::PageTable *PageTable;
#endif
    ControlBlockTime *Time;
    ThreadControlBlock *Threads[PROCESS_MAX_THREADS];
    uint32_t Checksum;
} ProcessControlBlock;

/**
 * @brief Get a running process by PID
 *
 * @param pid Process identifier
 * @return ProcessControlBlock
 */
EXTERNC ProcessControlBlock *FENAPI SysGetProcessByPID(uint64_t ID);

/**
 * @brief Get current process
 *
 * @return ProcessControlBlock
 */
EXTERNC ProcessControlBlock *FENAPI SysGetCurrentProcess();

/**
 * @brief Create a new process from a file
 *
 * @param name TODO: more
 * @return ProcessControlBlock
 */
EXTERNC ProcessControlBlock *FENAPI SysCreateProcessFromFile(const char *File); // TODO: complete

/**
 * @brief Create a new simple process with custom name and address space
 * 
 * @param Name 
 * @param PageTable 
 * @return ProcessControlBlock* 
 */
EXTERNC ProcessControlBlock *FENAPI SysCreateProcess(const char *Name, void *PageTable);

/**
 * @brief Create a new thread
 * 
 * @param Parent if null is using the current running
 * @param InstructionPointer 
 * @return ThreadControlBlock* 
 */
EXTERNC ThreadControlBlock *FENAPI SysCreateThread(ProcessControlBlock* Parent, uint64_t InstructionPointer);

#ifdef __cplusplus

enum TaskingMode
{
    None,
    Primitive,
    Advanced
};

extern int CurrentTaskingMode;

namespace PrimitiveTasking
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
         * @brief Construct a new Mono Tasking object
         *
         * @param firstThread The first Instruction Pointer to be executed
         */
        MonoTasking(uint64_t firstThread);
        /**
         * @brief Destroy the Mono Tasking object (Make sure that all other processes are destroyed and no stack is used!)
         *
         */
        ~MonoTasking();
        /**
         * @brief Create a new Task
         *
         * @param rip Instruction Pointer
         * @param args0 First argument
         * @param args1 Second argument
         * @param name The name of the new Task
         * @return The new created Task
         */
        TaskControlBlock *CreateTask(uint64_t rip, uint64_t args0, uint64_t args1, char *name);

    private:
    };

    extern MonoTasking SingleProcessing;
};

namespace AdvancedTasking
{
#define PROCESS_CHECKSUM 0xCAFEBABE
#define THREAD_CHECKSUM 0xDEADCAFE

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

    static ProcessControlBlock *ListProcess[MAX_PROCESSES];
    static ProcessControlBlock *CurrentProcess = nullptr;
    static ThreadControlBlock *CurrentThread = nullptr;
    static ProcessControlBlock *IdleProcess = nullptr;
    static ThreadControlBlock *IdleThread = nullptr;

    class MultiTasking
    {
    public:
        uint64_t NextPID = 0, NextTID = 0;
        void Schedule();
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

        ProcessControlBlock *GetCurrentProcess();
        ThreadControlBlock *GetCurrentThread();

        ProcessControlBlock *CreateProcess(ProcessControlBlock *parent, char *name);
        ThreadControlBlock *CreateThread(ProcessControlBlock *parent, uint64_t function, uint64_t args0, uint64_t args1, enum ControlBlockPriority Priority, enum ControlBlockState State, enum ControlBlockPolicy Policy);

    private:
    };
    extern MultiTasking MultiProcessing;
};

#endif


START_EXTERNC

void do_exit(uint64_t code);
void schedule();
int thread_page_fault_handler(REGISTERS *regs);

END_EXTERNC
