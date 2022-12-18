#pragma once
#include "ConnectionMessaging.hpp"
#include "RenderContext.hpp"

class ClientMessaging : public IConnectionMessaging
{
public:
    static ClientMessaging instance;

    void Start();
    void Stop();
    void SendRenderContextInfo(RenderContext*);

    DWORD managerProcessId = 0;

protected:
    void OnPacketReceived(PacketHeader packetType) override;
    void OnStreamOpened() override;
    void OnStreamClosed() override;

    void SendExistingRenderContexts();

private:
    void ThreadFunction();
    std::thread* thread = nullptr;
    std::mutex mutex;

    bool shouldThreadStop = false;
};
