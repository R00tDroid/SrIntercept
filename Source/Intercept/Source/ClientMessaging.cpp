#include "ClientMessaging.hpp"
#include "Log.hpp"

ClientMessaging ClientMessaging::instance;

void ClientMessaging::Start()
{
    shouldThreadStop = false;
    thread = new std::thread(&ClientMessaging::ThreadFunction, this);
}

void ClientMessaging::Stop()
{
    if (thread != nullptr) 
    {
        mutex.lock();
        shouldThreadStop = true;
        mutex.unlock();

        thread->join();
    }

    if (stream != nullptr)
    {
        delete stream;
        stream = nullptr;
    }
}

void ClientMessaging::OnPacketReceived(PacketHeader packetType)
{
    switch (packetType)
    {
        case PT_ManagerInfo:
        {
            managerProcessId = stream->Read<DWORD>();
            break;
        }
    }
}

void ClientMessaging::OnStreamOpened()
{
    logger.Log("Connected to SrIntercept");
}

void ClientMessaging::OnStreamClosed()
{
    logger.Log("Disconnected from SrIntercept");
}

void ClientMessaging::ThreadFunction()
{
    while (true) 
    {
        mutex.lock();
        if (shouldThreadStop)
        {
            mutex.unlock();
            break;
        }
        else 
        {
            mutex.unlock();
        }

        mutex.lock();
        if (stream == nullptr)
        {
            logger.Log("Connecting to SrIntercept...");
            stream = ClientConnectionStream::ConnectToHost("127.0.0.1", 5678);
        }
        mutex.unlock();

        mutex.lock();
        Update();
        mutex.unlock();
    }
}
