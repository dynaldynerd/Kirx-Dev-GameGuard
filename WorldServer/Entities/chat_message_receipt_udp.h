#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned _chat_message_receipt_udp
{
  unsigned __int8 byMessageType;
  unsigned int dwSenderSerial;
  char wszSenderName[17];
  unsigned __int8 byRaceCode;
  bool bFiltering;
  unsigned __int8 byPvpGrade;
  unsigned __int8 bySize;
  char wszChatData[256];

  _chat_message_receipt_udp();
  unsigned __int16 size() const;
};
#pragma pack(pop)

inline _chat_message_receipt_udp::_chat_message_receipt_udp()
{
  bySize = 0;
}

inline unsigned __int16 _chat_message_receipt_udp::size() const
{
  return static_cast<unsigned __int16>(282 - (255 - bySize));
}
