#include "ConnectionMessaging.hpp"

void IConnectionMessaging::Update()
{
    if (stream != nullptr)
    {
        while (stream->Available() >= sizeof(PacketHeader))
        {
            PacketHeader type = stream->Read<PacketHeader>();
            OnPacketReceived(type);
        }

        if (!stream->IsConnectionActive())
        {
            OnStreamClosed();
            delete stream;
            stream = nullptr;
        }
    }
}
