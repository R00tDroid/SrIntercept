#pragma once
#include "ConnectionStream.hpp"
#include "Packets.hpp"

class IConnectionMessaging
{
public:
    void Update();

protected:
    virtual void OnPacketReceived(PacketHeader packetType) = 0;
    virtual void OnStreamOpened() = 0;
    virtual void OnStreamClosed() = 0;

    IConnectionStream* stream = nullptr;
    bool wasPreviouslyActive = false;
};
