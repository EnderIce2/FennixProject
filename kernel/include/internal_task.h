#include <types.h>
#include <interrupts.h>
#include <heap.h>
#include <task.h>

namespace Tasking
{
    class Multitasking
    {
    private:
    public:
        Vector<PCB *> ListProcess;
        uint64_t NextPID = 0, NextTID = 0;

        PCB *IdleProcess = nullptr;
        TCB *IdleThread = nullptr;

        PCB *CreateProcess(PCB *Parent, char *Name, ELEVATION Elevation, int Priority = 100);
        TCB *CreateThread(PCB *Parent, uint64_t InstructionPointer, uint64_t Arg0, uint64_t Arg1,
                          int Priority = 100, enum Architecture Architecture = Architecture::x64, enum Platform Platform = Platform::Native);

        Multitasking();
        ~Multitasking();
    };

    extern bool MultitaskingSchedulerEnabled;
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

void StartTasking(uint64_t Address);

void init_syscalls();

END_EXTERNC
