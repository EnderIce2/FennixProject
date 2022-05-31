#include <vm.h>
#include <asm.h>
#include <cpuid.h>
#include <limits.h>

typedef struct
{
    union
    {
        uint32_t ax;
        uint32_t magic;
    };
    union
    {
        uint32_t bx;
        size_t size;
    };
    union
    {
        uint32_t cx;
        uint16_t command;
    };
    union
    {
        uint32_t dx;
        uint16_t port;
    };
    uint32_t si;
    uint32_t di;
} VMwareCMD;

#define VMWARE_HYPERVISOR_MAGIC 0x564D5868
#define VMWARE_HYPERVISOR_PORT 0x5658
#define VMWARE_HYPERVISOR_PORTHB 0x5659
#define VMWARE_PORT_CMD_GETVERSION 10

#define VMWARE_PORT(cmd, rax, rbx, rcx, rdx) \
    asm("inl (%%dx)"                         \
        : "=a"(rax),                         \
          "=c"(rcx),                         \
          "=d"(rdx),                         \
          "=b"(rbx)                          \
        : "0"(VMWARE_HYPERVISOR_MAGIC),      \
          "1"(VMWARE_PORT_CMD_##cmd),        \
          "2"(VMWARE_HYPERVISOR_PORT),       \
          "3"(UINT_MAX)                      \
        : "memory");

bool CheckRunningUnderVM()
{
    // cpu vendor check
    {
        uint32_t rax, rbx, rcx, rdx;
        char hyper_vendor_id[13];
        cpuid(0x40000000, &rax, &rbx, &rcx, &rdx);
        memcpy(hyper_vendor_id + 0, &rbx, 4);
        memcpy(hyper_vendor_id + 4, &rcx, 4);
        memcpy(hyper_vendor_id + 8, &rdx, 4);
        hyper_vendor_id[12] = '\0';
        debug("Vendor: %s", hyper_vendor_id);
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_KVM))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_VMWARE))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_VIRTUALBOX))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_XENHVM))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_MICROSOFT_HV))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_PARALLELS))
            return true;
        if (!strcmp(hyper_vendor_id, CPUID_VENDOR_TCG))
            return true;
        debug("CPU vendor check failed");
    }

    // vmware backdoor check
    {
        VMwareCMD cmd;
        cmd.bx = ~VMWARE_HYPERVISOR_MAGIC;
        cmd.command = VMWARE_PORT_CMD_GETVERSION;
        cmd.magic = VMWARE_HYPERVISOR_MAGIC;
        cmd.port = VMWARE_HYPERVISOR_PORT;

        asm volatile("in %%dx, %0"
                     : "+a"(cmd.ax),
                       "+b"(cmd.bx),
                       "+c"(cmd.cx),
                       "+d"(cmd.dx),
                       "+S"(cmd.si),
                       "+D"(cmd.di));

        if (cmd.bx == VMWARE_HYPERVISOR_MAGIC || cmd.ax != 0xFFFFFFFF)
            return true;
        debug("VMWARE backdoor check failed");
    }

    // vmware hypervisor check
    {
        uint32_t rax, rbx, rcx, rdx;
        VMWARE_PORT(GETVERSION, rax, rbx, rcx, rdx);
        debug("rbx: %#llx", rbx);
        if (rbx == VMWARE_HYPERVISOR_MAGIC)
            return true;
        debug("VMWARE hypervisor check failed");
    }

    // vmware dmi check
    {
        // char string[10];
        // TODO: not implemented
        // GetBIOSVendor(string);
        // if (!memcmp(string, "VMware-", 7) || !memcmp(string, "VMW", 3))
        //     return true;
        // debug("DMI check failed");
    }

    // check IDT
    {
        uint8_t m[6];
        asm("sidt %0"
            : "=m"(m));
        debug("IDTR: %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x", m[0], m[1], m[2], m[3], m[4], m[5]);
        if ((m[5] > 0xd0))
            return true;
        debug("IDT check failed");
        // return (m[5] > 0xd0) ? 1 : 0;
    }

    // check GDT
    {
        uint8_t m[6];
        asm("sgdt %0"
            : "=m"(m));
        debug("GDTR: %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x", m[0], m[1], m[2], m[3], m[4], m[5]);
        if ((m[5] > 0xd0))
            return true;
        debug("GDT check failed");
        // return (m[5] > 0xd0) ? 1 : 0;
    }

    // check LDT
    {
        uint8_t m[6];
        asm("sldt %0"
            : "=m"(m));
        debug("LDTR: %2.2x %2.2x %2.2x %2.2x %2.2x %2.2x",
              m[0], m[1], m[2], m[3], m[4], m[5]);
        if ((m[0] != 0x00 && m[1] != 0x00))
            return true;
        debug("LDT check failed");
        // return (m[0] != 0x00 && m[1] != 0x00) ? 1 : 0;
    }

    // check #1
    {
        uint32_t m[4];
        uint32_t rax, rbx, rcx, rdx;
        __cpuid((uint32_t *)m, rax, rbx, rcx, rdx);
        if (((m[2] >> 31) & 1) == 1)
            return true;
        debug("CPUID #1 check failed");
    }

    // check #2
    {
        uint8_t m[2 + 4], rpill[] = "\x0f\x01\x0d\x00\x00\x00\x00\xc3";
        *((unsigned *)&rpill[3]) = (unsigned long)m;
        ((void (*)()) & rpill)();
        if (m[5] > 0xd0)
            return true;
        debug("CPUID #2 check failed");
    }

    // check #3
    {
        // experimental

        uint32_t rax, rbx, rcx, rdx;
        // TODO: check with vmread instruction
    }

    // TODO: Somehow VirtualBox is still not detected in some circumstances.
    // TODO: Add more checks. (e.g. check for PCI)
    debug("No virtual machine detected");
    return false;
}
