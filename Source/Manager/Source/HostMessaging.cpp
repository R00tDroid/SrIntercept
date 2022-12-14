#include "HostMessaging.hpp"

#include "Log.hpp"

HostMessaging::HostMessaging(IConnectionStream* inStream)
{
    stream = inStream;

    shouldThreadStop = false;
    thread = new std::thread(&HostMessaging::ThreadFunction, this);
}

HostMessaging::~HostMessaging()
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

void HostMessaging::OnPacketReceived(PacketHeader packetType)
{
}

void HostMessaging::OnStreamClosed()
{
    logger.Log("Client disconnected");
}

void HostMessaging::ThreadFunction()
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
        Update();
        mutex.unlock();
    }
}
