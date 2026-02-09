#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _chat_steal_message_gm_zocl
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
  unsigned __int16 size() const;
};

inline _chat_steal_message_gm_zocl::_chat_steal_message_gm_zocl()
{
  bySize = 0;
}

inline unsigned __int16 _chat_steal_message_gm_zocl::size() const
{
  return static_cast<unsigned __int16>(282 - (255 - bySize));
}
