#pragma once
#include "ConnectionMessaging.hpp"

class HostMessaging : public IConnectionMessaging
{
public:
    HostMessaging(IConnectionStream* stream);
    ~HostMessaging();

    void SendManagerInfo();

protected:
    void OnPacketReceived(PacketHeader packetType) override;
    void OnStreamClosed() override;
    void OnStreamOpened() override {}

private:
    void ThreadFunction();

    std::thread* thread = nullptr;
    std::mutex mutex;

    bool shouldThreadStop = false;
};
