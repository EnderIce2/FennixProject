#include "NetworkController.hpp"

namespace NetworkUDP
{
    Socket::Socket(Provider *Provider)
    {
        netdbg("New socket created.");
        this->Info.SocketProvider = Provider;
    }

    Socket::~Socket()
    {
        netdbg("Socket destroyed.");
    }

    void Socket::Send(void *Data, uint64_t Length)
    {
        netdbg("Sending %d bytes.", Length);
        this->Info.SocketProvider->Send(this, Data, Length);
    }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    // Provider::Provider()
    // {
    //     netdbg("New provider created.");
    // }

    // Provider::~Provider()
    // {
    //     netdbg("Provider destroyed.");
    // }

    /* -------------------------------------------------------------------------------------------------------------------------------- */

    // Events::Events()
    // {
    //     netdbg("New events created.");
    // }

    // Events::~Events()
    // {
    //     netdbg("Events destroyed.");
    // }

    void OnReceive(Socket *Socket, void *Data, uint64_t Length)
    {
        warn("Unhandled message");
    }
}
