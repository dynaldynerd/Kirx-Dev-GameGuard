#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(2)) _chat_steal_message_gm_zocl
{
  unsigned __int8 byMessageType;
  unsigned int dwSenderSerial;
  char wszSenderName[17];
  unsigned __int8 byRaceCode;
  bool bFiltering;
  unsigned __int8 byPvpGrade;
  unsigned __int8 bySize;
  char wszChatData[256];

  _chat_steal_message_gm_zocl();
  __int64 size() const;
};
#pragma pack(pop)
