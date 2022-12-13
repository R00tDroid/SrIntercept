#pragma once
#include "ConnectionMessaging.hpp"

class ClientMessaging : public IConnectionMessaging
{
public:
    static ClientMessaging instance;

    void Start();
    void Stop();

protected:
    void OnPacketReceived(PacketHeader packetType) override;
    void OnStreamOpened() override;
    void OnStreamClosed() override;

private:
    void ThreadFunction();
    std::thread* thread = nullptr;
    std::mutex mutex;

    bool shouldThreadStop = false;
};
