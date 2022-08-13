#include "apic.hpp"

#include "../interrupts/pic.h"
#include "smp.hpp"

#include <internal_task.h>
#include <critical.hpp>
#include <symbols.hpp>
#include <int.h>
#include <io.h>

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

    // headache
    // https://www.amd.com/system/files/TechDocs/24593.pdf
    // https://www.naic.edu/~phil/software/intel/318148.pdf

    uint32_t APIC::Read(uint32_t Register)
    {
        // Too repetitive
        if (Register != APIC_EOI &&
            Register != APIC_TIMER &&
            Register != APIC_TDCR &&
            Register != APIC_TICR &&
            Register != APIC_TCCR)
            debug("APIC::Read(%#lx)", Register);
        if (x2APICSupported)
        {
            if (Register != APIC_ICRHI)
                return rdmsr((Register >> 4) + 0x800);
            else
                return rdmsr(0x30 + 0x800);
        }
        else
            return *((volatile uint32_t *)((uintptr_t)madt->LAPICAddr + Register));
    }

    void APIC::Write(uint32_t Register, uint32_t Value)
    {
// Too repetitive
#ifndef DEBUG_SCHEDULER
        if (Register != APIC_EOI &&
            Register != APIC_TIMER &&
            Register != APIC_TDCR &&
            Register != APIC_TICR &&
            Register != APIC_TCCR)
#endif
            debug("APIC::Write(%#lx, %#lx)", Register, Value);
        if (x2APICSupported)
        {
            if (Register != APIC_ICRHI)
                wrmsr((Register >> 4) + 0x800, Value);
            else
                wrmsr(MSR_X2APIC_ICR, Value);
        }
        else
            *((volatile uint32_t *)(((uintptr_t)madt->LAPICAddr) + Register)) = Value;
    }

    void APIC::IOWrite(uint64_t Base, uint32_t Register, uint32_t Value)
    {
        debug("APIC::IOWrite(%#lx, %#lx, %#lx)", Base, Register, Value);
        *((volatile uint32_t *)(((uintptr_t)Base))) = Register;
        *((volatile uint32_t *)(((uintptr_t)Base + 16))) = Value;
    }

    uint32_t APIC::IORead(uint64_t Base, uint32_t Register)
    {
        debug("APIC::IORead(%#lx, %#lx)", Base, Register);
        *((volatile uint32_t *)(((uintptr_t)Base))) = Register;
        return *((volatile uint32_t *)(((uintptr_t)Base + 16)));
    }

    void APIC::EOI()
    {
        this->Write(APIC_EOI, 0);
    }

    void APIC::RedirectIRQs(int CPU)
    {
        debug("redirecting irqs...");
        for (int i = 0; i < 16; i++)
            this->RedirectIRQ(CPU, i, 1);
        debug("redirecting irqs complete");
    }

    void APIC::IPI(uint8_t CPU, uint32_t InterruptNumber)
    {
        if (x2APICSupported)
        {
            wrmsr(MSR_X2APIC_ICR, ((uint64_t)CPU) << 32 | InterruptNumber);
        }
        else
        {
            InterruptNumber = (1 << 14) | InterruptNumber;
            this->Write(APIC_ICRHI, (CPU << 24));
            this->Write(APIC_ICRLO, InterruptNumber);
        }
    }

    void APIC::OneShot(uint32_t Vector, uint64_t Miliseconds)
    {
        apic->Write(APIC::APIC::APIC_TIMER, apic->Read(APIC::APIC::APIC_TIMER) & ~(1 << 0x10));
        this->Write(APIC_TDCR, 0x03);
        this->Write(APIC_TIMER, (APIC::APIC::APICRegisters::APIC_ONESHOT | Vector));
        this->Write(APIC_TICR, apic_timer_ticks * Miliseconds);
        apic->Write(APIC::APIC::APIC_TIMER, apic->Read(APIC::APIC::APIC_TIMER) | (1 << 0x10));
    }

    bool APIC::APICSupported()
    {
        if (!madt->LAPICAddr)
            return false;
        uint32_t rax, rbx, rcx, rdx;
        cpuid(1, &rax, &rbx, &rcx, &rdx);
        return (rdx & CPUID_FEAT_RDX_APIC);
    }

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
                debug("[ISO %d] Mapping to source IRQ%#d GSI:%#lx on CPU %d",
                      i, madt->iso[i]->IRQSource, madt->iso[i]->GSI, CPU);

                this->RawRedirectIRQ(madt->iso[i]->IRQSource + 0x20, madt->iso[i]->GSI, madt->iso[i]->Flags, CPU, Status);
                return;
            }
        debug("Mapping IRQ%d on CPU %d", IRQ, CPU);
        this->RawRedirectIRQ(IRQ + 0x20, IRQ, 0, CPU, Status);
    }

    APIC::APIC()
    {
        trace("Initializing APIC...");
        EnterCriticalSection;
        if (!this->APICSupported())
            return;

        uint32_t rax, rbx, rcx, rdx;
        cpuid(1, &rax, &rbx, &rcx, &rdx);
        if (rcx & CPUID_FEAT_RCX_x2APIC)
        {
            // trace("x2APIC Supported!");
            // x2APICSupported = true;
            trace("x2APIC is supported by the system but disabled because of an unknown error.");
            // wrmsr(MSR_APIC_BASE, (rdmsr(MSR_APIC_BASE) | (1 << 11)) & ~(1 << 10));
            wrmsr(MSR_APIC_BASE, (rdmsr(MSR_APIC_BASE) | (1 << 11)));
        }
        else
        {
            wrmsr(MSR_APIC_BASE, (rdmsr(MSR_APIC_BASE) | (1 << 11)));
        }

        debug("APIC Base Address is %#lx", rdmsr(MSR_APIC_BASE));

        this->Write(APIC_TPR, 0x0);
        this->Write(APIC_SVR, this->Read(APIC_SVR) | 0x100); // 0x1FF or 0x100 ? on https://wiki.osdev.org/APIC is 0x100

        if (!x2APICSupported)
        {
            this->Write(APIC_DFR, 0xF0000000);
            this->Write(APIC_LDR, this->Read(APIC_ID)); // APIC ID or 0xFF000000 ?
        }

        foreach (ACPI::MADT::MADTNmi *nmi in madt->nmi)
        {
            if (nmi->processor != 0xFF)
                if (nmi->processor == 0)
                {
                    uint32_t value = 0x400 | 2;
                    if (nmi->flags & 2)
                        value |= 1 << 13;
                    if (nmi->flags & 8)
                        value |= 1 << 15;
                    if (nmi->lint == 0)
                        this->Write(0x350, value);
                    else if (nmi->lint == 1)
                        this->Write(0x360, value);
                }
        }

        PIC_disable();
        LeaveCriticalSection;
    }

    APIC::~APIC()
    {
    }
}
