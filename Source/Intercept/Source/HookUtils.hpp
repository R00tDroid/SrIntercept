#pragma once
#include <detours.h>
#include <windows.h>

template<typename T>
void AttachFunction(const char* moduleName, const char* functionSignature, T& originalFunction, void* overrideFunction)
{
    AttachFunction<T>(DetourFindFunction(moduleName, functionSignature), originalFunction, overrideFunction);
}

template<typename T>
void AttachFunction(DWORD functionAddress, T& originalFunction, void* overrideFunction)
{
    AttachFunction<T>(reinterpret_cast<void*>(functionAddress), originalFunction, overrideFunction);
}

template<typename T>
void AttachFunction(void* target, T& originalFunction, void* overrideFunction)
{
    originalFunction = reinterpret_cast<T>(target);
    DetourAttach(&static_cast<void*>(originalFunction), overrideFunction);
}
