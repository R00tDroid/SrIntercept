#pragma once
#include "ConnectionStream.hpp"
#include <filesystem>
#include <softcam.h>
#include "HostMessaging.hpp"
#include <cmrc/cmrc.hpp>

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

    void OnClientConnected(HostConnectionStream* stream);

    std::filesystem::path binaryDirectory;
    std::filesystem::path dllPath;

    scCamera outputCamera = nullptr;

    HostConnection* connection = nullptr;

    std::vector<HostMessaging*> clientConnections;
};

extern cmrc::embedded_filesystem* resourceFS;
