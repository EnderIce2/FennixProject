#include "apic.hpp"
#include <int.h>
#include <io.h>
#include <critical.hpp>
#include "smp.hpp"

/* Inspired from: https://github.com/Supercip971/WingOS/blob/master/kernel/arch/x86_64/device/apic.cpp */

APIC::APIC *apic = nullptr;

namespace APIC
{
    enum IOAPICRegisters
    {
        GetIOAPICVersion = 0x1
    };

    enum IOAPICFlags
    {
        ActiveHighLow = 2,
        EdgeLevel = 8
    };

    struct IOAPICVersion
    {
        uint8_t Version;
        uint8_t Reserved;
        uint8_t MaximumRedirectionEntry;
        uint8_t Reserved2;
    };

    uint32_t APIC::IOGetMaxRedirect(uint32_t APICID)
    {
        uint32_t TableAddress = (this->IORead((madt->ioapic[APICID]->addr), GetIOAPICVersion));
        return ((IOAPICVersion *)&TableAddress)->MaximumRedirectionEntry;
    }

    void APIC::RawRedirectIRQ(uint8_t Vector, uint32_t GSI, uint16_t Flags, int CPU, int Status)
    {
        uint64_t end = Vector;

        int64_t io_apic_target = -1;
        for (uint64_t i = 0; madt->ioapic[i] != 0; i++)
            if (madt->ioapic[i]->gsib <= GSI)
                if (madt->ioapic[i]->gsib + IOGetMaxRedirect(i) > GSI)
                {
                    io_apic_target = i;
                    break;
                }

        if (io_apic_target == -1)
        {
            err("No ISO table found for I/O APIC");
            return;
        }

        if (Flags & ActiveHighLow)
            end |= (1 << 13);

        if (Flags & EdgeLevel)
            end |= (1 << 15);

        if (!Status)
            end |= (1 << 16);

        end |= (((uintptr_t)GetCPU(CPU)->LAPIC.APICId) << 56);
        uint32_t io_reg = (GSI - madt->ioapic[io_apic_target]->gsib) * 2 + 16;

        this->IOWrite(madt->ioapic[io_apic_target]->addr, io_reg, (uint32_t)end);
        this->IOWrite(madt->ioapic[io_apic_target]->addr, io_reg + 1, (uint32_t)(end >> 32));
    }

    void APIC::RedirectIRQ(int CPU, uint8_t IRQ, int Status)
    {
        for (uint64_t i = 0; i < madt->iso.size(); i++)
            if (madt->iso[i]->IRQSource == IRQ)
            {
                debug("(ReqRed IRQ%d) ISO %#lx mapping to IRQ Source:%#lx GSI:%#lx", IRQ, i, madt->iso[i]->IRQSource + 0x20, madt->iso[i]->GSI);
                this->RawRedirectIRQ(madt->iso[i]->IRQSource + 0x20, madt->iso[i]->GSI, madt->iso[i]->Flags, CPU, Status);
                return;
            }
        this->RawRedirectIRQ(IRQ + 0x20, IRQ, 0, CPU, Status);
    }

    APIC::APIC()
    {
        if (!this->APICSupported())
            return;

        wrmsr(MSR_APIC, (rdmsr(MSR_APIC) | 0x800) & ~(1 << 10));
        this->Write(APIC_SVR, this->Read(APIC_SVR) | 0x1FF);

        outb(PIC1_DATA, 0xff);
        do
        {
            asm volatile("jmp 1f\n\t"
                         "1:\n\t"
                         "    jmp 2f\n\t"
                         "2:");
        } while (0);
        outb(PIC2_DATA, 0xff);
    }

    APIC::~APIC()
    {
    }
}
