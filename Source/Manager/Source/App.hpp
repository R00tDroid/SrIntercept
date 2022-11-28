#pragma once
#include <filesystem>
#include <Log.hpp>

class SrInterceptManager
{
public:
    SrInterceptManager();
    bool Update();
    ~SrInterceptManager();

    Logger logger;

private:
    void StartDynamicInjection(std::filesystem::path executable);

    std::filesystem::path binaryDirectory;
    std::filesystem::path dllPath;
};
