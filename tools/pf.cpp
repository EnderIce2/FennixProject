#include <iostream>
#include <cstdint>

using namespace std;

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

int main(char argc, char **argv)
{
    system("clear");
    uint32_t err_code = 0;
    cout << "Page Fault e=";
    cin >> err_code;
    system("clear");
    printf("Page Fault e=%04d\n", err_code);
    PageFaultErrorCode params = {.raw = (uint32_t)err_code};
    printf("Page: %s\n", params.P ? "Present" : "Not Present");
    cout << "\033[1;31m      When set, the page fault was caused by a page-protection violation. When not set, it was caused by a non-present page.\033[0m\n\n";
    printf("Write Operation: %s\n", params.W ? "Read-Only" : "Read-Write");

    cout << "\033[1;31m      When set, the page fault was caused by a write access. When not set, it was caused by a read access.\033[0m\n\n";
    printf("Processor Mode: %s\n", params.U ? "User-Mode" : "Kernel-Mode");

    cout << "\033[1;31m      When set, the page fault was caused while CPL = 3. This does not necessarily mean that the page fault was a privilege violation.\033[0m\n\n";
    printf("CPU Reserved Bits: %s\n", params.R ? "Reserved" : "Unreserved");

    cout << "\033[1;31m      When set, one or more page directory entries contain reserved bits which are set to 1. This only applies when the PSE or PAE flags in CR4 are set to 1.\033[0m\n\n";
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
}
