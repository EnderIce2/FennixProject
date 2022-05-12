#include <types.h>
#include <interrupts.h>
#include <heap.h>
#include <msg.h>
#include <task.h>

enum TaskingMode
{
    None,
    Mono,
    Multi
};

extern int CurrentTaskingMode;

namespace Tasking
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

    class Monotasking
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
        Monotasking(uint64_t FirstTask);

        /**
         * @brief Destroy the Mono Tasking object (Make sure that all other processes are destroyed and no stack is used!)
         *
         */
        ~Monotasking();

    private:
    };

    class Multitasking
    {
    private:
    public:
        Vector<PCB *> ListProcess;
        uint64_t NextPID = 0, NextTID = 0;
        PCB *CurrentProcess = nullptr;
        TCB *CurrentThread = nullptr;

        PCB *IdleProcess = nullptr;
        TCB *IdleThread = nullptr;

        PCB *CreateProcess(PCB *Parent, char *Name, ELEVATION Elevation);
        TCB *CreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t Arg0, uint64_t Arg1);

        Multitasking();
        ~Multitasking();
    };

    extern bool MultitaskingSchedulerEnabled;
    extern Monotasking *monot;
    extern Multitasking *mt;
}

START_EXTERNC

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
 * @brief Create a new process from a file
 *
 * @param File TODO: more
 * @return PCB
 */
PCB *SysCreateProcessFromFile(const char *File, uint64_t arg0, uint64_t arg1, ELEVATION Elevation);

/**
 * @brief Create a new simple process with custom name and address space
 *
 * @param Name
 * @param PageTable
 * @return PCB*
 */
PCB *SysCreateProcess(const char *Name, ELEVATION Elevation);

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
