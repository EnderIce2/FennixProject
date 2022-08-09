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

    void OnReceive(NetworkUDP::Socket *Socket, uint8_t *Data, uint64_t Length)
    {
        fixme("OnReceive( %p %p %ld )", Socket, Data, Length);
    }

    void ReadTime()
    {
        fixme("ReadTime()");
    }
}
