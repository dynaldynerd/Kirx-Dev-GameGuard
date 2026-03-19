#include "pch.h"

#include "NetCheckPackets.h"

namespace
{
  unsigned int s_dwKey = 313210060;
  unsigned int s_KeyBuffer[4]{};
}

int _check_query::size() const
{
  return 1;
}

int _check_speed_hack_ans::size() const
{
  return 16;
}

unsigned int *CalcCodeKey(unsigned int *pdwCode)
{
  const unsigned int sum0 = pdwCode[0] + pdwCode[2];
  const unsigned int sum1 = pdwCode[1] + pdwCode[3];
  const unsigned int mix = sum0 - sum1;

  for (int i = 0; i < 4; ++i)
  {
    s_KeyBuffer[i] = (mix * s_dwKey) ^ pdwCode[i];
  }

  return s_KeyBuffer;
}
