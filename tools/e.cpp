#include <iostream>
#include <cstdint>

using namespace std;

#define clear system("clear")

typedef union CR0
{
    struct
    {
        /** @brief Protection Enable */
        uint64_t PE : 1;
        /** @brief Monitor Coprocessor */
        uint64_t MP : 1;
        /** @brief Emulation */
        uint64_t EM : 1;
        /** @brief Task Switched */
        uint64_t TS : 1;
        /** @brief Extension Type */
        uint64_t ET : 1;
        /** @brief Numeric Error */
        uint64_t NE : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 10;
        /** @brief Write Protect */
        uint64_t WP : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief Alignment Mask */
        uint64_t AM : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 10;
        /** @brief Mot Write-through */
        uint64_t NW : 1;
        /** @brief Cache Disable */
        uint64_t CD : 1;
        /** @brief Paging */
        uint64_t PG : 1;
    };
    uint64_t raw;
} CR0;

typedef union CR2
{
    struct
    {
        /** @brief Page Fault Linear Address */
        uint64_t PFLA;
    };
    uint64_t raw;
} CR2;

typedef union CR3
{
    struct
    {
        /** @brief Not used if bit 17 of CR4 is 1 */
        uint64_t PWT : 1;
        /** @brief Not used if bit 17 of CR4 is 1 */
        uint64_t PCD : 1;
        /** @brief Base of PML4T/PML5T */
        uint64_t PDBR;
    };
    uint64_t raw;
} CR3;

typedef union CR4
{
    struct
    {
        /** @brief Virtual-8086 Mode Extensions */
        uint64_t VME : 1;
        /** @brief Protected-Mode Virtual Interrupts */
        uint64_t PVI : 1;
        /** @brief Time Stamp Disable */
        uint64_t TSD : 1;
        /** @brief Debugging Extensions */
        uint64_t DE : 1;
        /** @brief Page Size Extensions */
        uint64_t PSE : 1;
        /** @brief Physical Address Extension */
        uint64_t PAE : 1;
        /** @brief Machine Check Enable */
        uint64_t MCE : 1;
        /** @brief Page Global Enable */
        uint64_t PGE : 1;
        /** @brief Performance Monitoring Counter */
        uint64_t PCE : 1;
        /** @brief Operating System Support */
        uint64_t OSFXSR : 1;
        /** @brief Operating System Support */
        uint64_t OSXMMEXCPT : 1;
        /** @brief User-Mode Instruction Prevention */
        uint64_t UMIP : 1;
        /** @brief Linear Address 57bit */
        uint64_t LA57 : 1;
        /** @brief VMX Enable */
        uint64_t VMXE : 1;
        /** @brief SMX Enable */
        uint64_t SMXE : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 1;
        /** @brief FSGSBASE Enable */
        uint64_t FSGSBASE : 1;
        /** @brief PCID Enable */
        uint64_t PCIDE : 1;
        /** @brief XSAVE and Processor Extended States Enable */
        uint64_t OSXSAVE : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief SMEP Enable */
        uint64_t SMEP : 1;
        /** @brief SMAP Enable */
        uint64_t SMAP : 1;
        /** @brief Protection-Key Enable */
        uint64_t PKE : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 9;
    };
    uint64_t raw;
} CR4;

typedef union
{
    struct
    {
        /** @brief Carry Flag */
        uint64_t CF : 1;
        /** @brief Reserved */
        uint64_t always_one : 1;
        /** @brief Parity Flag */
        uint64_t PF : 1;
        /** @brief Reserved */
        uint64_t _reserved0 : 1;
        /** @brief Auxiliary Carry Flag */
        uint64_t AF : 1;
        /** @brief Reserved */
        uint64_t _reserved1 : 1;
        /** @brief Zero Flag */
        uint64_t ZF : 1;
        /** @brief Sign Flag */
        uint64_t SF : 1;
        /** @brief Trap Flag */
        uint64_t TF : 1;
        /** @brief Interrupt Enable Flag */
        uint64_t IF : 1;
        /** @brief Direction Flag */
        uint64_t DF : 1;
        /** @brief Overflow Flag */
        uint64_t OF : 1;
        /** @brief I/O Privilege Level */
        uint64_t IOPL : 2;
        /** @brief Nested Task */
        uint64_t NT : 1;
        /** @brief Reserved */
        uint64_t _reserved2 : 1;
        /** @brief Resume Flag */
        uint64_t RF : 1;
        /** @brief Virtual 8086 Mode */
        uint64_t VM : 1;
        /** @brief Alignment Check */
        uint64_t AC : 1;
        /** @brief Virtual Interrupt Flag */
        uint64_t VIF : 1;
        /** @brief Virtual Interrupt Pending */
        uint64_t VIP : 1;
        /** @brief ID Flag */
        uint64_t ID : 1;
        /** @brief Reserved */
        uint64_t _reserved3 : 10;
    };
    uint64_t raw;
} Rrfl;

typedef union PageFaultErrorCode
{
    struct
    {
        uint64_t P : 1;
        uint64_t W : 1;
        uint64_t U : 1;
        uint64_t R : 1;
        uint64_t I : 1;
        uint64_t PK : 1;
        uint64_t SS : 1;
        uint64_t Reserved0 : 8;
        uint64_t SGX : 1;
        uint64_t Reserved1 : 16;
    };
    uint64_t raw;
} PageFaultErrorCode;

static const char *pagefault_message[] = {
    "Supervisory process tried to read a non-present page entry",
    "Supervisory process tried to read a page and caused a protection fault",
    "Supervisory process tried to write to a non-present page entry",
    "Supervisory process tried to write a page and caused a protection fault",
    "User process tried to read a non-present page entry",
    "User process tried to read a page and caused a protection fault",
    "User process tried to write to a non-present page entry",
    "User process tried to write a page and caused a protection fault"};

int main(char argc, char *argv[])
{
    clear;

    char crtype = 0;
    cout << "Enter error type [CR-0,2,3,4,f; Error-e; Help-h]: ";
    cin >> crtype;
    clear;

    CR0 cr0;
    CR2 cr2;
    CR3 cr3;
    CR4 cr4;
    Rrfl rfl;

    if (crtype == '0' || crtype == '2' || crtype == '3' || crtype == '4' || crtype == 'f')
    {
        cout << "Enter CR data: ";
        uint64_t raw;
        cin >> hex >> raw;
        cr0.raw = raw;
        cr2.raw = raw;
        cr3.raw = raw;
        cr4.raw = raw;
        rfl.raw = raw;
        clear;
    }

    switch (crtype)
    {
    case '0':
        printf("CR0: PE:%s     MP:%s     EM:%s     TS:%s\n     ET:%s     NE:%s     WP:%s     AM:%s\n     NW:%s     CD:%s     PG:%s\n     R0:%#x R1:%#x R2:%#x\n",
               cr0.PE ? "True " : "False", cr0.MP ? "True " : "False", cr0.EM ? "True " : "False", cr0.TS ? "True " : "False",
               cr0.ET ? "True " : "False", cr0.NE ? "True " : "False", cr0.WP ? "True " : "False", cr0.AM ? "True " : "False",
               cr0.NW ? "True " : "False", cr0.CD ? "True " : "False", cr0.PG ? "True " : "False",
               cr0._reserved0, cr0._reserved1, cr0._reserved2);
        break;
    case '2':
    {
        printf("CR2: PFLA: %#lx\n",
               cr2.PFLA);
        break;
    }
    break;
    case '3':
        printf("CR3: PWT:%s     PCD:%s    PDBR:%#lx\n",
               cr3.PWT ? "True " : "False", cr3.PCD ? "True " : "False", cr3.PDBR);
        break;
    case '4':
        printf("CR4: VME:%s     PVI:%s     TSD:%s      DE:%s\n     PSE:%s     PAE:%s     MCE:%s     PGE:%s\n     PCE:%s    UMIP:%s  OSFXSR:%s OSXMMEXCPT:%s\n    LA57:%s    VMXE:%s    SMXE:%s   PCIDE:%s\n OSXSAVE:%s    SMEP:%s    SMAP:%s     PKE:%s\n     R0:%#x R1:%#x R2:%#x\n",
               cr4.VME ? "True " : "False", cr4.PVI ? "True " : "False", cr4.TSD ? "True " : "False", cr4.DE ? "True " : "False",
               cr4.PSE ? "True " : "False", cr4.PAE ? "True " : "False", cr4.MCE ? "True " : "False", cr4.PGE ? "True " : "False",
               cr4.PCE ? "True " : "False", cr4.UMIP ? "True " : "False", cr4.OSFXSR ? "True " : "False", cr4.OSXMMEXCPT ? "True " : "False",
               cr4.LA57 ? "True " : "False", cr4.VMXE ? "True " : "False", cr4.SMXE ? "True " : "False", cr4.PCIDE ? "True " : "False",
               cr4.OSXSAVE ? "True " : "False", cr4.SMEP ? "True " : "False", cr4.SMAP ? "True " : "False", cr4.PKE ? "True " : "False",
               cr4._reserved0, cr4._reserved1, cr4._reserved2);
        break;
    case 'f':
        printf("RFL: CF:%s     PF:%s     AF:%s     ZF:%s\n     SF:%s     TF:%s     IF:%s     DF:%s\n     OF:%s   IOPL:%s     NT:%s     RF:%s\n     VM:%s     AC:%s    VIF:%s    VIP:%s\n     ID:%s     AlwaysOne:%d\n     R0:%#x R1:%#x R2:%#x R3:%#x\n",
           rfl.CF ? "True " : "False", rfl.PF ? "True " : "False", rfl.AF ? "True " : "False", rfl.ZF ? "True " : "False",
           rfl.SF ? "True " : "False", rfl.TF ? "True " : "False", rfl.IF ? "True " : "False", rfl.DF ? "True " : "False",
           rfl.OF ? "True " : "False", rfl.IOPL ? "True " : "False", rfl.NT ? "True " : "False", rfl.RF ? "True " : "False",
           rfl.VM ? "True " : "False", rfl.AC ? "True " : "False", rfl.VIF ? "True " : "False", rfl.VIP ? "True " : "False",
           rfl.ID ? "True " : "False", rfl.always_one,
           rfl._reserved0, rfl._reserved1, rfl._reserved2, rfl._reserved3);
        break;
    case 'e':
    {
        // TODO: support more error types
        uint32_t err_code = 0;
        cout << "[0xe Page Fault] e=";
        cin >> err_code;
        clear;
        printf("Page Fault e=%04d\n", err_code);
        PageFaultErrorCode params = {.raw = (uint32_t)err_code};
        printf("Page: %s\n", params.P ? "Present" : "Not Present");
        cout << "\033[1;31m      When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.\033[0m\n\n";
        printf("Write Operation: %s\n", params.W ? "Read-Only" : "Read-Write");

        cout << "\033[1;31m      When set, the page fault was caused by a write access. When not set, it was caused by a read access.\033[0m\n\n";
        printf("Processor Mode: %s\n", params.U ? "User-Mode" : "Kernel-Mode");

        cout << "\033[1;31m      When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.\033[0m\n\n";
        printf("CPU Reserved Bits: %s\n", params.R ? "Reserved" : "Unreserved");

        cout << "\033[1;31m      When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE rfl in CR4 are set to 1.\033[0m\n\n";
        printf("Caused By An Instruction Fetch: %s\n", params.I ? "Yes" : "No");

        cout << "\033[1;31m      When set, the page fault was caused by an instruction fetch. This only applies when the No-Execute bit is supported and enabled.\033[0m\n\n";
        printf("Caused By A Protection-Key Violation: %s\n", params.PK ? "Yes" : "No");

        cout << "\033[1;31m      When set, the page fault was caused by a protection-key violation. The PKRU register (for user-mode accesses) or PKRS MSR (for supervisor-mode accesses) specifies the protection key rights.\033[0m\n\n";
        printf("Caused By A Shadow Stack Access: %s\n", params.SS ? "Yes" : "No");

        cout << "\033[1;31m      When set, the page fault was caused by a shadow stack access.\033[0m\n\n";
        printf("Caused By An SGX Violation: %s\n", params.SGX ? "Yes" : "No");

        cout << "\033[1;31m      When set, the fault was due to an SGX violation. The fault is unrelated to ordinary paging.\033[0m\n\n";
        printf("Description: %s\n", err_code & 0x00000008 ? "One or more page directory entries contain reserved bits which are set to 1." : pagefault_message[err_code & 0b111]);

        cout << "\033[1;31m      Detailed exception\033[0m\n\n";
        printf("Reserved0: %#lx\n", params.Reserved0);

        cout << "\033[1;31m      Reserved\033[0m\n\n";
        printf("Reserved1: %#lx\n", params.Reserved1);

        cout << "\033[1;31m      Reserved\033[0m\n\n";
        break;
    }
    case 'h':
    {
        cout << "0,2,3,4 - CR data" << endl;
        cout << "e - Error code from QEMU" << endl;
        cout << "h - This text" << endl;
        break;
    }
    default:
        cout << "Invalid error type" << endl;
        break;
    }
    return 0;
}
