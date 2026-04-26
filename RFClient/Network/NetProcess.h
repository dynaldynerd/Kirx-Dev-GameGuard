#pragma once

#include <windows.h>

#define msg_header_num 2

#pragma pack(push, 1)

struct _MSG_HEADER
{
  WORD m_wSize;
  BYTE m_byType[msg_header_num];
};

#pragma pack(pop)

#define MSG_HEADER_SIZE sizeof(_MSG_HEADER)
