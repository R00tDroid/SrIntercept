#include "Log.hpp"
#include <cstdarg>

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int size = std::vsnprintf(nullptr, 0, format, args) + 1;
    if (size <= 0) return;
    char* buffer = (char*)malloc(size);
    std::vsnprintf(buffer, size, format, args);
    const std::string message(buffer);
    free(buffer);

    va_end(args);

    for (ILogSink* sink : sinks)
    {
        sink->Log(message);
    }
}
