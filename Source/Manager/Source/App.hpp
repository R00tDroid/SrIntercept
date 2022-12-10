#pragma once
#include "ConnectionStream.hpp"
#include <filesystem>
#include <Log.hpp>
#include <softcam.h>

class SrInterceptManager
{
public:
    SrInterceptManager();
    bool Update();
    ~SrInterceptManager();

private:
    void StartDynamicInjection(std::filesystem::path executable);
    void InstallWebcam();
    void UninstallWebcam();

    std::filesystem::path binaryDirectory;
    std::filesystem::path dllPath;

    scCamera outputCamera = nullptr;

    HostConnection* connection = nullptr;
};
