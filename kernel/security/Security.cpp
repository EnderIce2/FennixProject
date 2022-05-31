#include "security.hpp"

#include <internal_task.h>
#include <vector.hpp>
#include <rand.h>

struct TokenData
{
    uint64_t Token;
    enum TokenTrustLevel TrustLevel;
    uint64_t OwnerID;
    bool Process;
};

Vector<TokenData> Tokens;

uint64_t CreateToken()
{
    uint64_t ret = rand64();
    Tokens.push_back({ret, UnknownTrustLevel, 0, false});
    return ret;
}

void TrustToken(uint64_t Token, bool Process, uint64_t ID, int TrustLevel)
{
    enum TokenTrustLevel Level = static_cast<enum TokenTrustLevel>(TrustLevel);

    foreach (auto var in Tokens)
    {
        if (var.Token == Token)
        {
            var.TrustLevel = Level;
            var.OwnerID = ID;
            var.Process = Process;
            return;
        }
    }
}

bool CanSyscall(SyscallsRegs *regs)
{
    uint64_t TmpToken = SysGetCurrentThread()->Security.Token;

    fixme("CanSyscall( %p %ld )", regs, TmpToken);

    foreach (auto var in Tokens)
    {
        if (var.Token == TmpToken)
        {
            if (var.TrustLevel == TokenTrustLevel::UnknownTrustLevel)
            {
                return false;
            }
            else if (var.TrustLevel == TokenTrustLevel::TrustedByKernel)
            {
                return true;
            }
            else if (var.TrustLevel == TokenTrustLevel::Trusted)
            {
                return true;
            }
            else if (var.TrustLevel == TokenTrustLevel::Untrusted)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}
