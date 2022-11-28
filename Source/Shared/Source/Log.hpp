#pragma once
#include <string>
#include <vector>

class ILogSink
{
public:
    virtual void Log(std::string message) = 0;
};

class Logger
{
public:
    Logger();
    ~Logger();

    void Log(const char* format, ...);

    template<class T>
    T* AddSink()
    {
        T* newSink = new T();
        sinks.push_back(newSink);
        return newSink;
    }

private:
    std::vector< ILogSink*> sinks;
};
