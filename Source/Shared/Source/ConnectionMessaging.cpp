#include "ConnectionMessaging.hpp"

void IConnectionMessaging::Update()
{
    if (stream != nullptr)
    {
        if (!wasPreviouslyActive)
        {
            wasPreviouslyActive = true;
            OnStreamOpened();
        }

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
            wasPreviouslyActive = false;
        }
    }
}
