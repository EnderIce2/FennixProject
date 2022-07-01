#include <symbols.hpp>
#include <heap.h>
#include <string.h>
#include <elf.h>
#include <bootscreen.h>

#include "kernel.h"

#include "test.h"

#pragma GCC diagnostic ignored "-Wignored-qualifiers"

KernelSymbols::Symbols *SymTbl = nullptr;

namespace KernelSymbols
{
    Symbols::SymbolTable *SymTable = nullptr;
    uint64_t TotalEntries = 0;

    Symbols::Symbols()
    {
        Elf64_Ehdr *KernelHeader = (Elf64_Ehdr *)bootparams->kernel.file;
        Elf64_Shdr *ElfSections = (Elf64_Shdr *)(bootparams->kernel.file + KernelHeader->e_shoff);
        Elf64_Sym *ElfSymbols = nullptr;
        char *strtab = nullptr;

        for (uint64_t i = 0; i < KernelHeader->e_shnum; i++)
            switch (ElfSections[i].sh_type)
            {
            case SHT_SYMTAB:
                ElfSymbols = (Elf64_Sym *)(bootparams->kernel.file + ElfSections[i].sh_offset);
                TotalEntries = ElfSections[i].sh_size / sizeof(Elf64_Sym);
                debug("Symbol table found, %d entries", TotalEntries);
                break;
            case SHT_STRTAB:
                if (KernelHeader->e_shstrndx == i)
                {
                    debug("String table found, %d entries", ElfSections[i].sh_size);
                }
                else
                {
                    strtab = (char *)bootparams->kernel.file + ElfSections[i].sh_offset;
                    debug("String table found, %d entries", ElfSections[i].sh_size);
                }
                break;
            }

        if (ElfSymbols != nullptr && strtab != nullptr)
        {
            size_t Index, MinimumIndex;
            for (size_t i = 0; i < TotalEntries - 1; i++)
            {
                MinimumIndex = i;
                for (Index = i + 1; Index < TotalEntries; Index++)
                    if (ElfSymbols[Index].st_value < ElfSymbols[MinimumIndex].st_value)
                        MinimumIndex = Index;
                Elf64_Sym tmp = ElfSymbols[MinimumIndex];
                ElfSymbols[MinimumIndex] = ElfSymbols[i];
                ElfSymbols[i] = tmp;
            }

            while (ElfSymbols[0].st_value == 0)
            {
                ElfSymbols++;
                TotalEntries--;
            }

            trace("Symbol table loaded, %d entries (%ldKB)", TotalEntries, TO_KB(TotalEntries * sizeof(SymbolTable)));
            // TODO: broken?
            // SymTable = new SymbolTable[TotalEntries];
            SymTable = (SymbolTable *)KernelAllocator.RequestPages((TotalEntries * sizeof(SymbolTable)) / PAGE_SIZE + 1);
            // do_mem_test();

            for (size_t i = 0, g = TotalEntries; i < g; i++)
            {
                SymTable[i].Address = ElfSymbols[i].st_value;
                SymTable[i].FunctionName = &strtab[ElfSymbols[i].st_name];
            }
        }
        BS->IncreaseProgres();
    }

    Symbols::~Symbols()
    {
        // delete SymTable;
        KernelAllocator.FreePages(SymTable, (TotalEntries * sizeof(SymbolTable)) / PAGE_SIZE + 1);
    }

    string Symbols::GetSymbolFromAddress(uint64_t Address)
    {
        Symbols::SymbolTable Result{0, (char *)"<unknown>"};
        for (size_t i = 0; i < TotalEntries; i++)
            if (SymTable[i].Address <= Address && SymTable[i].Address > Result.Address)
                Result = SymTable[i];
        return Result.FunctionName;
    }
}
