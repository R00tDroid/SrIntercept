#pragma once

typedef unsigned short PacketHeader;

enum PacketType : PacketHeader
{
    PT_ManagerInfo, // 1 DWORD (manager processId)
    PT_RenderContextInfo, // 1 HANDLE (texture share handle)
};
