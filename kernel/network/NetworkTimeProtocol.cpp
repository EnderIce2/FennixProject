#include "NetworkController.hpp"

namespace NetworkNTP
{
    NTP::NTP(NetworkUDP::Socket *Socket)
    {
    }

    NTP::~NTP()
    {
    }

    void OnUDPMessageReceived(NetworkUDP::Socket *Socket, void *Data, uint64_t Length)
    {
        fixme("OnUDPMessageReceived( %p %p %ld )", Socket, Data, Length);
    }

    void ReadTime()
    {
        fixme("ReadTime()");
    }
}
