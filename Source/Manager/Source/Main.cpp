#include "App.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
    SrInterceptManager app;
    while (app.Update()) { }
    return S_OK;
}
