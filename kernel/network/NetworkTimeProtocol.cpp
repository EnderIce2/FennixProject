#include "NetworkController.hpp"

namespace NetworkNTP
{
    // NTP::NTP(NetworkUDP::Socket *Socket)
    // {
    //     this->Socket = Socket;
    // }

    // NTP::~NTP()
    // {
    // }

    void OnReceive(NetworkUDP::Socket *Socket, void *Data, uint64_t Length)
    {
        fixme("OnReceive( %p %p %ld )", Socket, Data, Length);
    }

    void ReadTime()
    {
        fixme("ReadTime()");
    }
}
