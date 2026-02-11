#include "pch.h"

#include "ECONOMY_SYSTEM.h"

#include <mmsystem.h>

_ECONOMY_SYSTEM::_ECONOMY_SYSTEM()
{
  Init();
}

void _ECONOMY_SYSTEM::Init()
{
  m_bLoad = false;
  CurTradeMoneyInit();
  m_dwLastUpdateTime = timeGetTime();
  m_dwSystemOperStartTime = timeGetTime();
}

void _ECONOMY_SYSTEM::CurTradeMoneyInit()
{
  for (int j = 0; j < 3; ++j)
  {
    m_dCurTradeGold[j] = DOUBLE_1_0;
    m_dCurTradeDalant[j] = DOUBLE_1_0;
    for (int k = 0; k < 3; ++k)
    {
      m_dCurOreMineCount[j][k] = DOUBLE_1_0;
      m_dCurOreCutCount[j][k] = DOUBLE_1_0;
    }
  }
}
