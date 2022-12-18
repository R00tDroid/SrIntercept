#include "ConnectionStream.hpp"
#include <functional>
#include <Log.hpp>

IConnectionStream::IConnectionStream(CSimpleSocket* inSocket) : socket(inSocket)
{
    thread = new std::thread(&IConnectionStream::ThreadFunction, this);
}

IConnectionStream::~IConnectionStream()
{
    if (socket != nullptr) 
    {
        socket->Close();
        delete socket;
        socket = nullptr;
    }
}

void IConnectionStream::BeginWrite(PacketHeader packetId)
{
    writeLock.lock();
    Write(packetId);
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

    incomingLock.lock();
    while (incomingData.size() < dataSize)
    {
        // Release lock temporarily to allow thread to provide more data
        incomingLock.unlock();
        incomingLock.lock();
    }

    memcpy(buffer, incomingData.data(), dataSize);
    incomingData.erase(incomingData.begin(), incomingData.begin() + dataSize);
    incomingLock.unlock();
}

unsigned short IConnectionStream::Available()
{
    incomingLock.lock();
    unsigned short size = incomingData.size();
    incomingLock.unlock();
    return size;
}

bool IConnectionStream::IsConnectionActive()
{
    threadLock.lock();
    bool status = thread != nullptr;
    threadLock.unlock();
    return status;
}

void IConnectionStream::ThreadFunction()
{
    while (true)
    {
        int received = socket->Receive();

        if (received == 0) continue; 
        if (received < 0) break; // Client has disconnected

        for (int i = 0; i < socket->GetBytesReceived(); i++)
        {
            incomingLock.lock();
            incomingData.push_back(socket->GetData()[i]);
            incomingLock.unlock();
        }
    }

    logger.Log("TCP incoming thread has stopped");

    threadLock.lock();
    thread = nullptr;
    threadLock.unlock();
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

            if (onConnectionOpened != nullptr)
            {
                onConnectionOpened(stream);
            }
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

HostConnectionStream::HostConnectionStream(CSimpleSocket* inSocket) : IConnectionStream(inSocket)
{
    logger.Log("TCP client connected");
}

ClientConnectionStream* ClientConnectionStream::ConnectToHost(std::string address, unsigned short port)
{
    logger.Log("Connecting to TCP host: %s:%i", address.c_str(), port);

    CActiveSocket* clientSocket = new CActiveSocket();
    clientSocket->Initialize();
    if (clientSocket->Open(address.c_str(), port))
    {
        return new ClientConnectionStream(clientSocket);
    }
    else
    {
        delete clientSocket;
        logger.Log("Failed to connect to TCP host");
        return nullptr;
    }
}

ClientConnectionStream::ClientConnectionStream(CSimpleSocket* socket) : IConnectionStream(socket)
{
    logger.Log("Connected to TCP host");
}
