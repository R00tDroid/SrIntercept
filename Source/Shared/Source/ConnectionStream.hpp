#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <SimpleSocket.h>
#include <PassiveSocket.h>
#include <mutex>
#include <string>
#include <vector>

class HostConnectionStream;

class IConnectionStream
{
public:
    IConnectionStream(CSimpleSocket* socket);
    ~IConnectionStream();

    void BeginWrite();
    void EndWrite();

    void Write(void* buffer, unsigned int elementSize, unsigned int count);

    template<class T>
    void Write(T* buffer, unsigned int count = 1)
    {
        Write(buffer, sizeof(T), count);
    }

    template<class T>
    void Write(T value)
    {
        Write(&value);
    }

    void Read(void* buffer, unsigned int elementSize, unsigned int count);

    template<class T>
    void Read(T* buffer, unsigned int count = 1)
    {
        Read(buffer, sizeof(T), count);
    }

    template<class T>
    T Read()
    {
        T value;
        Read(&value);
        return value;
    }

    unsigned short Available();

protected:
    void ThreadFunction();

private:
    CSimpleSocket* socket = nullptr;
    std::mutex writeLock;

    std::mutex incomingLock;
    std::vector<unsigned char> incomingData;

    std::thread* thread = nullptr;
};

class HostConnection
{
public:
    HostConnection(std::string address, unsigned short port);

    void ThreadFunction();

    std::vector<HostConnectionStream*> GetStreams();

private:
    CPassiveSocket* socket = nullptr;
    std::thread* thread = nullptr;

    std::mutex streamsLock;
    std::vector<HostConnectionStream*> streams;
};

class HostConnectionStream : public IConnectionStream
{
public:
    HostConnectionStream(CSimpleSocket* socket);
};

class ClientConnectionStream : public IConnectionStream
{
public:
    static ClientConnectionStream* ConnectToHost(std::string address, unsigned short port);

private:
    ClientConnectionStream(CSimpleSocket* socket);
};
