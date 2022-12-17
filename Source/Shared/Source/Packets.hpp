#pragma once

typedef unsigned short PacketHeader;

enum PacketType : PacketHeader
{
    PT_ManagerInfo // 1 DWORD (manager processId)
};
