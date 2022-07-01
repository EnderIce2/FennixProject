#pragma once
#include <types.h>

namespace KernelSymbols
{
    class Symbols
    {
    public:
        struct SymbolTable
        {
            uint64_t Address;
            char *FunctionName;
        };

        Symbols();
        ~Symbols();
        string GetSymbolFromAddress(uint64_t Address);
    };
}

extern KernelSymbols::Symbols *SymTbl;
