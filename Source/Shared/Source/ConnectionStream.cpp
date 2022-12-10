#include "ConnectionStream.hpp"
#include <functional>
#include <Log.hpp>

IConnectionStream::~IConnectionStream()
{
    if (socket != nullptr) 
    {
        socket->Close();
        delete socket;
        socket = nullptr;
    }
}

void IConnectionStream::BeginWrite()
{
    writeLock.lock();
}

void IConnectionStream::EndWrite()
{
    writeLock.unlock();
}

void IConnectionStream::Write(void* buffer, unsigned elementSize, unsigned count)
{
    socket->Send(static_cast<uint8*>(buffer), elementSize * count);
}

void IConnectionStream::Read(void* buffer, unsigned elementSize, unsigned count)
{
    unsigned int dataSize = elementSize * count;
    while (incomingData.size() < dataSize) { }
    memcpy(buffer, incomingData.data(), dataSize);
    incomingData.erase(incomingData.begin(), incomingData.begin() + dataSize);
}

HostConnection::HostConnection(std::string address, unsigned short port)
{
    socket = new CPassiveSocket();
    socket->Initialize();
    if (socket->Listen(address.c_str(), port)) 
    {
        logger.Log("Listening for TCP clients: %s:%i", address.c_str(), port);
    }
    else
    {
        logger.Log("Failed to open TCP host: %s:%i", address.c_str(), port);
    }

    thread = new std::thread(std::bind(&HostConnection::ThreadFunction, this));
}

void HostConnection::ThreadFunction()
{
    while (true) 
    {
        CActiveSocket* incomingSocket = nullptr;
        if ((incomingSocket = socket->Accept()) != nullptr)
        {
            HostConnectionStream* stream = new HostConnectionStream(incomingSocket);

            streamsLock.lock();
            streams.push_back(stream);
            streamsLock.unlock();
        }
    }
}

std::vector<HostConnectionStream*> HostConnection::GetStreams()
{
    streamsLock.lock();
    std::vector<HostConnectionStream*> copy = streams ;
    streamsLock.unlock();
    return copy;
}

HostConnectionStream::HostConnectionStream(CSimpleSocket* inSocket)
{
    socket = inSocket;
    logger.Log("TCP client connected");
}

ClientConnectionStream::ClientConnectionStream(std::string address, unsigned short port)
{
    logger.Log("Connecting to TCP host: %s:%i", address.c_str(), port);

    CActiveSocket* clientSocket = new CActiveSocket();
    socket = clientSocket;
    clientSocket->Initialize();
    if (clientSocket->Open(address.c_str(), port))
    {
        logger.Log("Connected to TCP host");
    }
    else
    {
        logger.Log("Failed to connect to TCP host");
    }
}
