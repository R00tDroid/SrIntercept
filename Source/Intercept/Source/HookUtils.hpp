#pragma once
#include <detours.h>
#include <windows.h>

typedef void* FunctionPointer;

FunctionPointer FindFunction(const char* moduleName, const char* functionSignature)
{
    return DetourFindFunction(moduleName, functionSignature);
}

template<typename T>
void AttachFunction(FunctionPointer target, T& originalFunction, void* overrideFunction)
{
    originalFunction = reinterpret_cast<T>(target);
    DetourAttach(&static_cast<void*>(originalFunction), overrideFunction);
}
