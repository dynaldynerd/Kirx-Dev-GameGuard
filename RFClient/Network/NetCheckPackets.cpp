#include "Network/NetCheckPackets.h"

namespace
{
DWORD s_dwKey = 313210060;
DWORD s_KeyBuffer[CHECK_KEY_NUM]{};
} // namespace

int _check_query::size() const
{
  return sizeof(*this);
}

int _check_speed_hack_ans::size() const
{
  return sizeof(*this);
}

DWORD *CalcCodeKey(DWORD *pdwCode)
{
  const DWORD l_dwSum0 = pdwCode[0] + pdwCode[2];
  const DWORD l_dwSum1 = pdwCode[1] + pdwCode[3];
  const DWORD l_dwMix = l_dwSum0 - l_dwSum1;

  for (int i = 0; i < CHECK_KEY_NUM; ++i)
  {
    s_KeyBuffer[i] = (l_dwMix * s_dwKey) ^ pdwCode[i];
  }

  return s_KeyBuffer;
}
