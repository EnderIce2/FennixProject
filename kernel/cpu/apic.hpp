#pragma once
#include <types.h>
#include <cpuid.h>
#include <asm.h>
#include "acpi.hpp"
#include "../timer/apic_timer.h"

namespace APIC
{
    class APIC
    {
    public:
        enum APICRegisters
        {
            APIC_ONESHOT = (0 << 17),      // LVT One-Shot Mode  (for Timer)
            APIC_PERIODIC = (1 << 17),     // LVT Periodic Mode (for Timer)
            APIC_TSC_DEADLINE = (2 << 17), // LVT Timer/sDeadline (for Timer)
            // source from: https://github.com/pdoane/osdev/blob/master/intr/local_apic.c
            APIC_ID = 0x20,       // Local APIC ID
            APIC_VER = 0x30,      // Local APIC Version
            APIC_TPR = 0x80,      // Task Priority
            APIC_APR = 0x90,      // Arbitration Priority
            APIC_PPR = 0xa0,      // Processor Priority
            APIC_EOI = 0xb0,      // EOI
            APIC_RRD = 0xc0,      // Remote Read
            APIC_LDR = 0xd0,      // Logical Destination
            APIC_DFR = 0xe0,      // Destination Format
            APIC_SVR = 0xf0,      // Spurious Interrupt Vector
            APIC_ISR = 0x100,     // In-Service (8 registers)
            APIC_TMR = 0x180,     // Trigger Mode (8 registers)
            APIC_IRR = 0x200,     // Interrupt Request (8 registers)
            APIC_ESR = 0x280,     // Error Status
            APIC_ICRLO = 0x300,   // Interrupt Command
            APIC_ICRHI = 0x310,   // Interrupt Command [63:32]
            APIC_TIMER = 0x320,   // LVT Timer
            APIC_THERMAL = 0x330, // LVT Thermal Sensor
            APIC_PERF = 0x340,    // LVT Performance Counter
            APIC_LINT0 = 0x350,   // LVT LINT0
            APIC_LINT1 = 0x360,   // LVT LINT1
            APIC_ERROR = 0x370,   // LVT Error
            APIC_TICR = 0x380,    // Initial Count (for Timer)
            APIC_TCCR = 0x390,    // Current Count (for Timer)
            APIC_TDCR = 0x3e0,    // Divide Configuration (for Timer)
        };

        uint32_t Read(APICRegisters Register)
        {
            return *((volatile uint32_t *)((uintptr_t)madt->LAPICAddr + Register));
        }

        void Write(APICRegisters Register, uint32_t Value)
        {
            *((volatile uint32_t *)(((uintptr_t)madt->LAPICAddr) + Register)) = Value;
        }

        void IOWrite(uint64_t Base, uint32_t Register, uint32_t Value)
        {
            *((volatile uint32_t *)(((uintptr_t)Base))) = Register;
            *((volatile uint32_t *)(((uintptr_t)Base + 16))) = Value;
        }

        uint32_t IORead(uint64_t Base, uint32_t Register)
        {
            *((volatile uint32_t *)(((uintptr_t)Base))) = Register;
            return *((volatile uint32_t *)(((uintptr_t)Base + 16)));
        }

        void EOI()
        {
            this->Write(APIC_EOI, 0);
        }

        void RedirectIRQs(int CPU = 0)
        {
            debug("redirecting irqs...");
            for (int i = 0; i < 16; i++)
                this->RedirectIRQ(CPU, i, 1);
            debug("redirecting irqs complete");
        }

        void IPI(uint8_t CPU, uint32_t InterruptNumber)
        {
            InterruptNumber = (1 << 14) | InterruptNumber;
            this->Write(APIC_ICRHI, (CPU << 24));
            this->Write(APIC_ICRLO, InterruptNumber);
        }

        void OneShot(uint32_t Vector, uint64_t Miliseconds)
        {
            this->Write(APIC_TDCR, 0x03);
            this->Write(APIC_TIMER, (APIC::APIC::APICRegisters::APIC_ONESHOT | Vector));
            this->Write(APIC_TICR, apic_timer_ticks * Miliseconds);
        }

        bool APICSupported()
        {
            if (!madt->LAPICAddr)
                return false;
            uint32_t rax, rbx, rcx, rdx;
            cpuid(1, &rax, &rbx, &rcx, &rdx);
            return (rdx & CPUID_FEAT_RDX_APIC);
        }

        uint32_t IOGetMaxRedirect(uint32_t APICID);
        void RawRedirectIRQ(uint8_t Vector, uint32_t GSI, uint16_t Flags, int CPU, int Status);
        void RedirectIRQ(int CPU, uint8_t IRQ, int Status);
        APIC();
        ~APIC();
    };
}

extern APIC::APIC *apic;
