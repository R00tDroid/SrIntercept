#include "ConnectionStream.hpp"

#include <functional>

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
    socket->Listen(address.c_str(), port);

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
}

ClientConnectionStream::ClientConnectionStream(std::string address, unsigned short port)
{
    CActiveSocket* clientSocket = new CActiveSocket();
    socket = clientSocket;
    clientSocket->Initialize();
    clientSocket->Open(address.c_str(), port);
}
