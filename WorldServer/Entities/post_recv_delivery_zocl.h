#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _post_recv_delivery_zocl
{
  unsigned __int8 byIndex;
  unsigned int dwPostSerial;
  char wszSendName[17];
  char wszTitle[21];
  bool bItem;
  bool bGold;
  unsigned __int8 byState;

  unsigned __int16 size() const;
};

inline unsigned __int16 _post_recv_delivery_zocl::size() const
{
  return static_cast<unsigned __int16>(sizeof(_post_recv_delivery_zocl));
}
