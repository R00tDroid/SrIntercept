#include "Log.hpp"
#include <cstdarg>
#include <iostream>
#include <Windows.h>

Logger logger;

class CoutSink : ILogSink
{
public:
    void Log(std::string message) override
    {
        std::cout << message.c_str() << std::endl;
    }
};

class DebugOutSink : ILogSink
{
public:
    void Log(std::string message) override
    {
        OutputDebugStringA((message + '\n').c_str());
    }
};

Logger::Logger()
{
    AddSink<CoutSink>();
    AddSink<DebugOutSink>();
}

Logger::~Logger()
{
    for (ILogSink* sink : sinks)
    {
        delete sink;
    }
    sinks.clear();
}

void Logger::Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int size = std::vsnprintf(nullptr, 0, format, args) + 1;
    if (size <= 0) return;
    char* buffer = (char*)malloc(size);
    std::vsnprintf(buffer, size, format, args);
    const std::string message(prefix + buffer);
    free(buffer);

    va_end(args);

    for (ILogSink* sink : sinks)
    {
        sink->Log(message);
    }
}

void Logger::SetPrefix(std::string inPrefix)
{
    prefix = inPrefix;
}
