#pragma once
#include <string>
#include <vector>

class ILogSink
{
public:
    virtual ~ILogSink() = default;
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
        sinks.push_back((ILogSink*)newSink);
        return newSink;
    }

private:
    std::vector<ILogSink*> sinks;
};
