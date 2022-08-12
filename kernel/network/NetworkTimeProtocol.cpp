#include "NetworkController.hpp"

namespace NetworkNTP
{
    NTP::NTP(NetworkUDP::Socket *Socket) : NetworkUDP::UDPEvents()
    {
        this->Socket = Socket;
    }

    NTP::~NTP()
    {
    }

    void ReadTime()
    {
        fixme("ReadTime()");
    }
}
