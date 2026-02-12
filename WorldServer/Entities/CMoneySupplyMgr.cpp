#include "pch.h"

#include "CMoneySupplyMgr.h"

#include <new>
#include <mmsystem.h>

CMoneySupplyMgr *CMoneySupplyMgr::pInstance = nullptr;

namespace
{
void UpdateClassCounter(_MONEY_SUPPLY_DATA::_money_supply &counter, char *szClass)
{
  if (!strcmp_0(szClass, "BWB0"))
  {
    ++counter.nClass[0];
  }
  else if (!strcmp_0(szClass, "BWF1"))
  {
    ++counter.nClass[1];
  }
  else if (!strcmp_0(szClass, "BWF2"))
  {
    ++counter.nClass[2];
  }
  else if (!strcmp_0(szClass, "BWS1"))
  {
    ++counter.nClass[3];
  }
  else if (!strcmp_0(szClass, "BWS2"))
  {
    ++counter.nClass[4];
  }
  else if (!strcmp_0(szClass, "BWS3"))
  {
    ++counter.nClass[5];
  }
  else if (!strcmp_0(szClass, "BRB0"))
  {
    ++counter.nClass[6];
  }
  else if (!strcmp_0(szClass, "BRF1"))
  {
    ++counter.nClass[7];
  }
  else if (!strcmp_0(szClass, "BRF2"))
  {
    ++counter.nClass[8];
  }
  else if (!strcmp_0(szClass, "BRS1"))
  {
    ++counter.nClass[9];
  }
  else if (!strcmp_0(szClass, "BRS2"))
  {
    ++counter.nClass[10];
  }
  else if (!strcmp_0(szClass, "BRS3"))
  {
    ++counter.nClass[11];
  }
  else if (!strcmp_0(szClass, "BFB0"))
  {
    ++counter.nClass[12];
  }
  else if (!strcmp_0(szClass, "BFF1"))
  {
    ++counter.nClass[13];
  }
  else if (!strcmp_0(szClass, "BFF2"))
  {
    ++counter.nClass[14];
  }
  else if (!strcmp_0(szClass, "BFS1"))
  {
    ++counter.nClass[15];
  }
  else if (!strcmp_0(szClass, "BFS2"))
  {
    ++counter.nClass[16];
  }
  else if (!strcmp_0(szClass, "BFS3"))
  {
    ++counter.nClass[17];
  }
  else if (!strcmp_0(szClass, "BSB0"))
  {
    ++counter.nClass[18];
  }
  else if (!strcmp_0(szClass, "BSF1"))
  {
    ++counter.nClass[19];
  }
  else if (!strcmp_0(szClass, "BSF2"))
  {
    ++counter.nClass[20];
  }
  else if (!strcmp_0(szClass, "BSS1"))
  {
    ++counter.nClass[21];
  }
  else if (!strcmp_0(szClass, "BSS2"))
  {
    ++counter.nClass[22];
  }
  else if (!strcmp_0(szClass, "CWB0"))
  {
    ++counter.nClass[23];
  }
  else if (!strcmp_0(szClass, "CWF1"))
  {
    ++counter.nClass[24];
  }
  else if (!strcmp_0(szClass, "CWF2"))
  {
    ++counter.nClass[25];
  }
  else if (!strcmp_0(szClass, "CWS1"))
  {
    ++counter.nClass[26];
  }
  else if (!strcmp_0(szClass, "CWS2"))
  {
    ++counter.nClass[27];
  }
  else if (!strcmp_0(szClass, "CWS3"))
  {
    ++counter.nClass[28];
  }
  else if (!strcmp_0(szClass, "CRB0"))
  {
    ++counter.nClass[29];
  }
  else if (!strcmp_0(szClass, "CRF1"))
  {
    ++counter.nClass[30];
  }
  else if (!strcmp_0(szClass, "CRF2"))
  {
    ++counter.nClass[31];
  }
  else if (!strcmp_0(szClass, "CRS1"))
  {
    ++counter.nClass[32];
  }
  else if (!strcmp_0(szClass, "CRS2"))
  {
    ++counter.nClass[33];
  }
  else if (!strcmp_0(szClass, "CRS3"))
  {
    ++counter.nClass[34];
  }
  else if (!strcmp_0(szClass, "CFB0"))
  {
    ++counter.nClass[35];
  }
  else if (!strcmp_0(szClass, "CFF1"))
  {
    ++counter.nClass[36];
  }
  else if (!strcmp_0(szClass, "CFF2"))
  {
    ++counter.nClass[37];
  }
  else if (!strcmp_0(szClass, "CFS1"))
  {
    ++counter.nClass[38];
  }
  else if (!strcmp_0(szClass, "CFS2"))
  {
    ++counter.nClass[39];
  }
  else if (!strcmp_0(szClass, "CFS3"))
  {
    ++counter.nClass[40];
  }
  else if (!strcmp_0(szClass, "CSB0"))
  {
    ++counter.nClass[41];
  }
  else if (!strcmp_0(szClass, "CSF1"))
  {
    ++counter.nClass[42];
  }
  else if (!strcmp_0(szClass, "CSS1"))
  {
    ++counter.nClass[43];
  }
  else if (!strcmp_0(szClass, "AWB0"))
  {
    ++counter.nClass[44];
  }
  else if (!strcmp_0(szClass, "AWF1"))
  {
    ++counter.nClass[45];
  }
  else if (!strcmp_0(szClass, "AWF2"))
  {
    ++counter.nClass[46];
  }
  else if (!strcmp_0(szClass, "AWS1"))
  {
    ++counter.nClass[47];
  }
  else if (!strcmp_0(szClass, "AWS2"))
  {
    ++counter.nClass[48];
  }
  else if (!strcmp_0(szClass, "AWS3"))
  {
    ++counter.nClass[49];
  }
  else if (!strcmp_0(szClass, "ARB0"))
  {
    ++counter.nClass[50];
  }
  else if (!strcmp_0(szClass, "ARF1"))
  {
    ++counter.nClass[51];
  }
  else if (!strcmp_0(szClass, "ARF2"))
  {
    ++counter.nClass[52];
  }
  else if (!strcmp_0(szClass, "ARS1"))
  {
    ++counter.nClass[53];
  }
  else if (!strcmp_0(szClass, "ARS2"))
  {
    ++counter.nClass[54];
  }
  else if (!strcmp_0(szClass, "ARS3"))
  {
    ++counter.nClass[55];
  }
  else if (!strcmp_0(szClass, "ASB0"))
  {
    ++counter.nClass[56];
  }
  else if (!strcmp_0(szClass, "ASF1"))
  {
    ++counter.nClass[57];
  }
  else if (!strcmp_0(szClass, "ASS1"))
  {
    ++counter.nClass[58];
  }
  else if (!strcmp_0(szClass, "ASS2"))
  {
    ++counter.nClass[59];
  }
}
}

void _MONEY_SUPPLY_DATA::init()
{
  memset_0(this, 0, sizeof(*this));
}

CMoneySupplyMgr::CMoneySupplyMgr() = default;

CMoneySupplyMgr *CMoneySupplyMgr::Instance()
{
  if (!pInstance)
  {
    pInstance = new (std::nothrow) CMoneySupplyMgr();
  }
  return pInstance;
}

void CMoneySupplyMgr::Initialize()
{
  m_dwLastSendTime = timeGetTime();
  m_dwSystemOperStartTime = timeGetTime();
  m_MS_data.init();
}

void CMoneySupplyMgr::UpdateBuyData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount)
{
  m_MS_data.dwAmount[4] += nAmount;
  const int index = 3;

  switch (nLv)
  {
    case 30:
      ++m_MS_data.ms_data[index].nLv[0];
      break;
    case 40:
      ++m_MS_data.ms_data[index].nLv[1];
      break;
    case 50:
      ++m_MS_data.ms_data[index].nLv[2];
      break;
    case 60:
      ++m_MS_data.ms_data[index].nLv[3];
      break;
    default:
      break;
  }

  if (!byRace)
  {
    ++m_MS_data.ms_data[index].nRace[0];
  }
  if (byRace == 1)
  {
    ++m_MS_data.ms_data[index].nRace[1];
  }
  else
  {
    ++m_MS_data.ms_data[index].nRace[2];
  }

  UpdateClassCounter(m_MS_data.ms_data[index], szClass);
}

void CMoneySupplyMgr::UpdateSellData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount)
{
  m_MS_data.dwAmount[0] += nAmount;

  switch (nLv)
  {
    case 30:
      ++m_MS_data.ms_data[0].nLv[0];
      break;
    case 40:
      ++m_MS_data.ms_data[0].nLv[1];
      break;
    case 50:
      ++m_MS_data.ms_data[0].nLv[2];
      break;
    case 60:
      ++m_MS_data.ms_data[0].nLv[3];
      break;
    default:
      break;
  }

  if (!byRace)
  {
    ++m_MS_data.ms_data[0].nRace[0];
  }
  if (byRace == 1)
  {
    ++m_MS_data.ms_data[0].nRace[1];
  }
  else
  {
    ++m_MS_data.ms_data[0].nRace[2];
  }

  UpdateClassCounter(m_MS_data.ms_data[0], szClass);
}

void CMoneySupplyMgr::UpdateQuestRewardMoneyData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount)
{
  m_MS_data.dwAmount[1] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.ms_data[1].nLv[0];
      break;
    case 40:
      ++m_MS_data.ms_data[1].nLv[1];
      break;
    case 50:
      ++m_MS_data.ms_data[1].nLv[2];
      break;
    case 60:
      ++m_MS_data.ms_data[1].nLv[3];
      break;
    default:
      break;
  }

  if (!byRace)
  {
    ++m_MS_data.ms_data[1].nRace[0];
  }
  if (byRace == 1)
  {
    ++m_MS_data.ms_data[1].nRace[1];
  }
  else
  {
    ++m_MS_data.ms_data[1].nRace[2];
  }

  if (!strcmp_0(szClass, "BWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[0];
  }
  else if (!strcmp_0(szClass, "BWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[4];
  }
  else if (!strcmp_0(szClass, "BWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[5];
  }
  else if (!strcmp_0(szClass, "BRB0"))
  {
    ++m_MS_data.ms_data[1].nClass[6];
  }
  else if (!strcmp_0(szClass, "BRF1"))
  {
    ++m_MS_data.ms_data[1].nClass[7];
  }
  else if (!strcmp_0(szClass, "BRF2"))
  {
    ++m_MS_data.ms_data[1].nClass[8];
  }
  else if (!strcmp_0(szClass, "BRS1"))
  {
    ++m_MS_data.ms_data[1].nClass[9];
  }
  else if (!strcmp_0(szClass, "BRS2"))
  {
    ++m_MS_data.ms_data[1].nClass[10];
  }
  else if (!strcmp_0(szClass, "BRS3"))
  {
    ++m_MS_data.ms_data[1].nClass[11];
  }
  else if (!strcmp_0(szClass, "BFB0"))
  {
    ++m_MS_data.ms_data[1].nClass[12];
  }
  else if (!strcmp_0(szClass, "BFF1"))
  {
    ++m_MS_data.ms_data[1].nClass[13];
  }
  else if (!strcmp_0(szClass, "BFF2"))
  {
    ++m_MS_data.ms_data[1].nClass[14];
  }
  else if (!strcmp_0(szClass, "BFS1"))
  {
    ++m_MS_data.ms_data[1].nClass[15];
  }
  else if (!strcmp_0(szClass, "BFS2"))
  {
    ++m_MS_data.ms_data[1].nClass[16];
  }
  else if (!strcmp_0(szClass, "BFS3"))
  {
    ++m_MS_data.ms_data[1].nClass[17];
  }
  else if (!strcmp_0(szClass, "BSB0"))
  {
    ++m_MS_data.ms_data[1].nClass[18];
  }
  else if (!strcmp_0(szClass, "BSF1"))
  {
    ++m_MS_data.ms_data[1].nClass[19];
  }
  else if (!strcmp_0(szClass, "BSF2"))
  {
    ++m_MS_data.ms_data[1].nClass[20];
  }
  else if (!strcmp_0(szClass, "BSS1"))
  {
    ++m_MS_data.ms_data[1].nClass[21];
  }
  else if (!strcmp_0(szClass, "BSS2"))
  {
    ++m_MS_data.ms_data[1].nClass[22];
  }
  else if (!strcmp_0(szClass, "CWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[23];
  }
  else if (!strcmp_0(szClass, "CWF1"))
  {
    ++m_MS_data.ms_data[1].nClass[24];
  }
  else if (!strcmp_0(szClass, "CWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[25];
  }
  else if (!strcmp_0(szClass, "CWS1"))
  {
    ++m_MS_data.ms_data[1].nClass[26];
  }
  else if (!strcmp_0(szClass, "CWS2"))
  {
    ++m_MS_data.ms_data[1].nClass[27];
  }
  else if (!strcmp_0(szClass, "CWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[28];
  }
  else if (!strcmp_0(szClass, "CRB0"))
  {
    ++m_MS_data.ms_data[1].nClass[29];
  }
  else if (!strcmp_0(szClass, "CRF1"))
  {
    ++m_MS_data.ms_data[1].nClass[30];
  }
  else if (!strcmp_0(szClass, "CRF2"))
  {
    ++m_MS_data.ms_data[1].nClass[31];
  }
  else if (!strcmp_0(szClass, "CRS1"))
  {
    ++m_MS_data.ms_data[1].nClass[32];
  }
  else if (!strcmp_0(szClass, "CRS2"))
  {
    ++m_MS_data.ms_data[1].nClass[33];
  }
  else if (!strcmp_0(szClass, "CRS3"))
  {
    ++m_MS_data.ms_data[1].nClass[34];
  }
  else if (!strcmp_0(szClass, "CFB0"))
  {
    ++m_MS_data.ms_data[1].nClass[35];
  }
  else if (!strcmp_0(szClass, "CFF1"))
  {
    ++m_MS_data.ms_data[1].nClass[36];
  }
  else if (!strcmp_0(szClass, "CFF2"))
  {
    ++m_MS_data.ms_data[1].nClass[37];
  }
  else if (!strcmp_0(szClass, "CFS1"))
  {
    ++m_MS_data.ms_data[1].nClass[38];
  }
  else if (!strcmp_0(szClass, "CFS2"))
  {
    ++m_MS_data.ms_data[1].nClass[39];
  }
  else if (!strcmp_0(szClass, "CFS3"))
  {
    ++m_MS_data.ms_data[1].nClass[40];
  }
  else if (!strcmp_0(szClass, "CSB0"))
  {
    ++m_MS_data.ms_data[1].nClass[41];
  }
  else if (!strcmp_0(szClass, "CSF1"))
  {
    ++m_MS_data.ms_data[1].nClass[42];
  }
  else if (!strcmp_0(szClass, "CSS1"))
  {
    ++m_MS_data.ms_data[1].nClass[43];
  }
  else if (!strcmp_0(szClass, "AWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[44];
  }
  else if (!strcmp_0(szClass, "AWF1"))
  {
    ++m_MS_data.ms_data[1].nClass[45];
  }
  else if (!strcmp_0(szClass, "AWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[46];
  }
  else if (!strcmp_0(szClass, "AWS1"))
  {
    ++m_MS_data.ms_data[1].nClass[47];
  }
  else if (!strcmp_0(szClass, "AWS2"))
  {
    ++m_MS_data.ms_data[1].nClass[48];
  }
  else if (!strcmp_0(szClass, "AWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[49];
  }
  else if (!strcmp_0(szClass, "ARB0"))
  {
    ++m_MS_data.ms_data[1].nClass[50];
  }
  else if (!strcmp_0(szClass, "ARF1"))
  {
    ++m_MS_data.ms_data[1].nClass[51];
  }
  else if (!strcmp_0(szClass, "ARF2"))
  {
    ++m_MS_data.ms_data[1].nClass[52];
  }
  else if (!strcmp_0(szClass, "ARS1"))
  {
    ++m_MS_data.ms_data[1].nClass[53];
  }
  else if (!strcmp_0(szClass, "ARS2"))
  {
    ++m_MS_data.ms_data[1].nClass[54];
  }
  else if (!strcmp_0(szClass, "ARS3"))
  {
    ++m_MS_data.ms_data[1].nClass[55];
  }
  else if (!strcmp_0(szClass, "ASB0"))
  {
    ++m_MS_data.ms_data[1].nClass[56];
  }
  else if (!strcmp_0(szClass, "ASF1"))
  {
    ++m_MS_data.ms_data[1].nClass[57];
  }
  else if (!strcmp_0(szClass, "ASS1"))
  {
    ++m_MS_data.ms_data[1].nClass[58];
  }
  else if (!strcmp_0(szClass, "ASS2"))
  {
    ++m_MS_data.ms_data[1].nClass[59];
  }
}

void CMoneySupplyMgr::UpdateBuyUnitData(int nLv, unsigned int nAmount)
{
  m_MS_data.dwAmount[7] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.nBuyUnitLv[0];
      break;
    case 40:
      ++m_MS_data.nBuyUnitLv[1];
      break;
    case 50:
      ++m_MS_data.nBuyUnitLv[2];
      break;
    case 60:
      ++m_MS_data.nBuyUnitLv[3];
      break;
    default:
      break;
  }
}

void CMoneySupplyMgr::UpdateUnitRepairingChargesData(int nLv, unsigned int nAmount)
{
  m_MS_data.dwAmount[8] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.nUnitRepairLv[0];
      break;
    case 40:
      ++m_MS_data.nUnitRepairLv[1];
      break;
    case 50:
      ++m_MS_data.nUnitRepairLv[2];
      break;
    case 60:
      ++m_MS_data.nUnitRepairLv[3];
      break;
    default:
      break;
  }
}

void CMoneySupplyMgr::UpdateFeeMoneyData(unsigned __int8 byRace, int nLv, unsigned int nAmount)
{
  m_MS_data.dwAmount[5] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.nFeeLv[0];
      break;
    case 40:
      ++m_MS_data.nFeeLv[1];
      break;
    case 50:
      ++m_MS_data.nFeeLv[2];
      break;
    case 60:
      ++m_MS_data.nFeeLv[3];
      break;
    default:
      break;
  }

  if (!byRace)
  {
    ++m_MS_data.nFeeRace[0];
  }
  if (byRace == 1)
  {
    ++m_MS_data.nFeeRace[1];
  }
  else
  {
    ++m_MS_data.nFeeRace[2];
  }
}

void CMoneySupplyMgr::UpdateHonorGuildMoneyData(
  unsigned __int8 byTradeType,
  unsigned __int8 byRace,
  unsigned int nAmount)
{
  if (byTradeType)
  {
    if (byTradeType == 1)
    {
      m_MS_data.dwAmount[6] += nAmount;
      if (!byRace)
      {
        ++m_MS_data.nHonorGuildRace[byTradeType][0];
      }
      if (byRace == 1)
      {
        ++m_MS_data.nHonorGuildRace[byTradeType][1];
      }
      else
      {
        ++m_MS_data.nHonorGuildRace[byTradeType][2];
      }
    }
  }
  else
  {
    m_MS_data.dwAmount[3] += nAmount;
    if (!byRace)
    {
      ++m_MS_data.nHonorGuildRace[0][0];
    }
    if (byRace == 1)
    {
      ++m_MS_data.nHonorGuildRace[0][1];
    }
    else
    {
      ++m_MS_data.nHonorGuildRace[0][2];
    }
  }
}

