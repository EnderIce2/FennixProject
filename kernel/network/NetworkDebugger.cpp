#include "NetworkController.hpp"

#ifdef DEBUG_NETWORK

#include <stdarg.h>
#include <printf.h>
#include "../drivers/serial.h"

namespace NetDbg
{
    class NETWORK_DEBUG : public NetworkInterfaceManager::Events
    {
    public:
        static inline void print_wrapper(char c, void *unused) { write_serial(COM1, c); }
        int vprintf(const char *format, va_list list) { return vfctprintf(print_wrapper, NULL, format, list); }
        void WriteRaw(const char *format, ...)
        {
            va_list args;
            va_start(args, format);
            vprintf(format, args);
            va_end(args);
        }

        void DumpData(const char *Description, void *Address, unsigned long Length)
        {
            WriteRaw("-------------------------------------------------------------------------\n");
            unsigned char *AddressChar = (unsigned char *)Address;
            unsigned char Buffer[17];
            unsigned long Iterate;

            if (Description != nullptr)
                WriteRaw("%s:\n", Description);

            for (Iterate = 0; Iterate < Length; Iterate++)
            {
                if ((Iterate % 16) == 0)
                {
                    if (Iterate != 0)
                        WriteRaw("  %s\n", Buffer);
                    WriteRaw("  %04x ", Iterate);
                }

                WriteRaw(" %02x", AddressChar[Iterate]);

                if ((AddressChar[Iterate] < 0x20) || (AddressChar[Iterate] > 0x7e))
                    Buffer[Iterate % 16] = '.';
                else
                    Buffer[Iterate % 16] = AddressChar[Iterate];

                Buffer[(Iterate % 16) + 1] = '\0';
            }

            while ((Iterate % 16) != 0)
            {
                WriteRaw("   ");
                Iterate++;
            }

            WriteRaw("  %s\n", Buffer);
            WriteRaw("-------------------------------------------------------------------------\n");
        }

        NETWORK_DEBUG() : NetworkInterfaceManager::Events(nullptr) { netdbg("NetworkDebugger initialized."); }
        ~NETWORK_DEBUG() { netdbg("NetworkDebugger destroyed."); }
        virtual void OnInterfaceReceived(NetworkInterfaceManager::DeviceInterface *Interface, uint8_t *Data, uint64_t Length) { DumpData("Received", Data, Length); }
        virtual void OnInterfaceSent(NetworkInterfaceManager::DeviceInterface *Interface, uint8_t *Data, uint64_t Length) { DumpData("Sent", Data, Length); }
    };
}

NetDbg::NETWORK_DEBUG *N;
void DbgNetwork() { N = new NetDbg::NETWORK_DEBUG; }
void DbgDumpData(const char *Description, void *Address, unsigned long Length) { N->DumpData(Description, Address, Length); }

#endif
