#include "pch.h"

#include "CCheckSumCharacTrunkConverter.h"

#include "CCheckSumTypes.h"
#include "CCheckSumCharacAccountTrunkData.h"

void CCheckSumCharacTrunkConverter::Convert(_AVATOR_DATA *pAvator, CCheckSumCharacAccountTrunkData *pkCheckSum)
{
  if (!pAvator)
  {
    return;
  }

  unsigned int value = ProcCode(0, pkCheckSum->m_dwSerial, pAvator->dbAvator.m_dwDalant);
  pkCheckSum->SetValue(CDWT_DALANT, value);

  value = ProcCode(1u, pkCheckSum->m_dwSerial, pAvator->dbAvator.m_dwGold);
  pkCheckSum->SetValue(CDWT_GOLD, value);

  value = ProcCode(2u, pkCheckSum->m_dwSerial, static_cast<unsigned int>(pAvator->dbAvator.m_byLevel));
  pkCheckSum->SetValue(CDWT_LV, value);

  value = ProcCode(3u, pkCheckSum->m_dwSerial, pAvator->dbStat.m_dwDamWpCnt[0]);
  pkCheckSum->SetValue(CDWT_NEAR_MASTERY, value);

  value = ProcCode(4u, pkCheckSum->m_dwSerial, pAvator->dbStat.m_dwDefenceCnt);
  pkCheckSum->SetValue(CDWT_DIS_MASTERY, value);

  value = ProcCode(5u, pkCheckSum->m_dwSerial, pAvator->dbStat.m_dwDamWpCnt[1]);
  pkCheckSum->SetValue(CDWT_DEF_MASTERY, value);

  long double trunkDalant = ProcCode(
    static_cast<unsigned __int8>(2 * pkCheckSum->m_byRace),
    pkCheckSum->m_dwAccountSerial,
    pAvator->dbTrunk.dDalant);
  pkCheckSum->SetValue(CDT_TRUNK_DALANT, trunkDalant);

  long double trunkGold = ProcCode(
    static_cast<unsigned __int8>(2 * pkCheckSum->m_byRace + 1),
    pkCheckSum->m_dwAccountSerial,
    pAvator->dbTrunk.dGold);
  pkCheckSum->SetValue(CDT_TRUNK_GOLD, trunkGold);
}

void CCheckSumCharacTrunkConverter::ConvertTrunk(unsigned int dwSerial, long double *pVal)
{
  if (!pVal)
  {
    return;
  }

  for (int j = 0; j < 6; ++j)
  {
    pVal[j] = ProcCode(static_cast<unsigned __int8>(j), dwSerial, pVal[j]);
  }
}

