#pragma once

#include "IdaCompat.h"

struct _chat_multi_far_trans_zocl
{
  unsigned __int16 wSize;
  char sData[500];

  _chat_multi_far_trans_zocl();
  unsigned __int16 size();
};

inline _chat_multi_far_trans_zocl::_chat_multi_far_trans_zocl()
{
  wSize = 0;
}

inline unsigned __int16 _chat_multi_far_trans_zocl::size()
{
  if (wSize > 0x1F4u)
  {
    wSize = 0;
  }
  return static_cast<unsigned __int16>(502 - (500 - wSize));
}
