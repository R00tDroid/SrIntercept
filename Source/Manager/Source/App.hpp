#pragma once
#include <filesystem>

class SrInterceptManager
{
public:
    SrInterceptManager();
    bool Update();
    ~SrInterceptManager();

private:
    void StartDynamicInjection(std::filesystem::path executable);

    std::filesystem::path binaryDirectory;
    std::filesystem::path dllPath;
};
