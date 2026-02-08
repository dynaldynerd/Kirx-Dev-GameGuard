#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _chat_message_receipt_udp
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

inline _chat_message_receipt_udp::_chat_message_receipt_udp()
{
  bySize = 0;
}

inline unsigned __int16 _chat_message_receipt_udp::size() const
{
  return static_cast<unsigned __int16>(sizeof(_chat_message_receipt_udp));
}
