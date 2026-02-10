#include "pch.h"

#include "CCheckSumGuildConverter.h"

#include "CCheckSumTypes.h"
#include "CCheckSumGuildData.h"

void CCheckSumGuildConverter::Convert(long double dDalant, long double dGold, CCheckSumGuildData *pkCheckSum)
{
  long double dalant = ProcCode(6u, pkCheckSum->m_dwGuildSerial, dDalant);
  pkCheckSum->SetValue(CDT_TRUNK_DALANT, dalant);

  long double gold = ProcCode(7u, pkCheckSum->m_dwGuildSerial, dGold);
  pkCheckSum->SetValue(CDT_TRUNK_GOLD, gold);
}
