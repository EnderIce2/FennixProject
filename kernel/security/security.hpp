#pragma once

#include "../syscalls/syscalls.hpp"

uint64_t CreateToken();
void TrustToken(uint64_t Token, bool Process, uint64_t ID, /* enum TokenTrustLevel */ int TrustLevel);
bool CanSyscall(SyscallsRegs *regs);
