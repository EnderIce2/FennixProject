#include <symbols.hpp>
#include <heap.h>
#include <string.h>
#include <elf.h>
#include <bootscreen.h>

#include "kernel.h"

#pragma GCC diagnostic ignored "-Wignored-qualifiers"

KernelSymbols::Symbols *SymTbl = nullptr;

namespace KernelSymbols
{
    Symbols::SymbolTable *SymTable;
    size_t TotalEntries = 0;

    // TODO: fix symbol table
    Symbols::Symbols()
    {
        Elf64_Ehdr *KernelHeader = (Elf64_Ehdr *)bootparams->kernel.file;
        Elf64_Shdr *ElfSections = (Elf64_Shdr *)(bootparams->kernel.file + KernelHeader->e_shoff);
        Elf64_Sym *ElfSymbols;
        const char *const strtab = (const char *const)((Elf64_Off)KernelHeader + (Elf64_Off)(&ElfSections[KernelHeader->e_shstrndx])->sh_offset);

        for (size_t i = 0; i < KernelHeader->e_shnum; i++)
            switch (ElfSections[i].sh_type)
            {
            case SHT_SYMTAB:
                ElfSymbols = (Elf64_Sym *)(bootparams->kernel.file + ElfSections[i].sh_offset);
                TotalEntries = ElfSections[i].sh_size / ElfSections[i].sh_entsize;
                debug("Symbol table found, %d entries", TotalEntries);
                break;
            case SHT_STRTAB:
                // I can put in strtab the string table? I can use conversion to char* and remove the const from strtab.
                debug("String table found at %p", (bootparams->kernel.file + ElfSections[i].sh_offset));
                break;
            }

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
        SymTable = static_cast<SymbolTable *>(kcalloc(TotalEntries, sizeof(SymbolTable)));
#ifdef DEBUG
        static uint32_t empty_names = 0;
#endif
        for (size_t i = 0, g = TotalEntries; i < g; i++)
        {
            SymTable[i].Address = ElfSymbols[i].st_value;
            SymTable[i].FunctionName = &strtab[ElfSymbols[i].st_name];
#ifdef DEBUG
            if (isempty((char *)SymTable[i].FunctionName))
            {
                empty_names++;
            }
            else
            {
                // debug("Testing Symbols %p -> Name: \"%s\"", SymTable[i].Address, SymTable[i].FunctionName);
            }
        }
        debug("Empty or Non valid symbols: %d", empty_names);
#else
        }
#endif
        BS->IncreaseProgres();
    }

    Symbols::~Symbols()
    {
    }

    string Symbols::GetSymbolFromAddress(uint64_t Address)
    {
        Symbols::SymbolTable Result{0, "<unknown>"};
        for (size_t i = 0; i < TotalEntries; i++)
        {
            if (SymTable[i].Address <= Address && SymTable[i].Address > Result.Address)
                Result = SymTable[i];
        }
        return Result.FunctionName;
    }

}
