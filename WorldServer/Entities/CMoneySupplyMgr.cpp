#include "pch.h"

#include "CMoneySupplyMgr.h"

#include <new>
#include <mmsystem.h>

#include "Packet/ZoneWebPacket.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CMoneySupplyMgr *CMoneySupplyMgr::pInstance;

void _MONEY_SUPPLY_DATA::init()
{
  std::memset(this, 0, sizeof(*this));
}

CMoneySupplyMgr::CMoneySupplyMgr() = default;

CMoneySupplyMgr::~CMoneySupplyMgr()
{
  // this is not a stub
}

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

void CMoneySupplyMgr::SendMsg_MoneySupplyDataToWeb(_MONEY_SUPPLY_DATA *pMSData)
{
  _money_supply_gatering_inform_zowb packet{};
  std::memcpy(&packet, pMSData, sizeof(packet));

  unsigned __int8 type[2]{51, 20};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      type,
      reinterpret_cast<char *>(&packet),
      static_cast<unsigned __int16>(sizeof(packet)));
  }
}

void CMoneySupplyMgr::LoopMoneySupply()
{
  if (!g_Main.m_bWorldOpen)
  {
    return;
  }

  const unsigned int loopTime = GetLoopTime();
  if (loopTime >= m_dwLastSendTime + 60000)
  {
    m_MS_Senddata.init();
    std::memcpy(&m_MS_Senddata, &m_MS_data, sizeof(_MONEY_SUPPLY_DATA));
    m_MS_data.init();
    SendMsg_MoneySupplyDataToWeb(&m_MS_Senddata);
    m_dwLastSendTime = loopTime;
  }
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

  if (!std::strcmp(szClass, "BWB0"))
    {
      ++m_MS_data.ms_data[index].nClass[0];
    }
    else if (!std::strcmp(szClass, "BWF1"))
    {
      ++m_MS_data.ms_data[index].nClass[1];
    }
    else if (!std::strcmp(szClass, "BWF2"))
    {
      ++m_MS_data.ms_data[index].nClass[2];
    }
    else if (!std::strcmp(szClass, "BWS1"))
    {
      ++m_MS_data.ms_data[index].nClass[3];
    }
    else if (!std::strcmp(szClass, "BWS2"))
    {
      ++m_MS_data.ms_data[index].nClass[4];
    }
    else if (!std::strcmp(szClass, "BWS3"))
    {
      ++m_MS_data.ms_data[index].nClass[5];
    }
    else if (!std::strcmp(szClass, "BRB0"))
    {
      ++m_MS_data.ms_data[index].nClass[6];
    }
    else if (!std::strcmp(szClass, "BRF1"))
    {
      ++m_MS_data.ms_data[index].nClass[7];
    }
    else if (!std::strcmp(szClass, "BRF2"))
    {
      ++m_MS_data.ms_data[index].nClass[8];
    }
    else if (!std::strcmp(szClass, "BRS1"))
    {
      ++m_MS_data.ms_data[index].nClass[9];
    }
    else if (!std::strcmp(szClass, "BRS2"))
    {
      ++m_MS_data.ms_data[index].nClass[10];
    }
    else if (!std::strcmp(szClass, "BRS3"))
    {
      ++m_MS_data.ms_data[index].nClass[11];
    }
    else if (!std::strcmp(szClass, "BFB0"))
    {
      ++m_MS_data.ms_data[index].nClass[12];
    }
    else if (!std::strcmp(szClass, "BFF1"))
    {
      ++m_MS_data.ms_data[index].nClass[13];
    }
    else if (!std::strcmp(szClass, "BFF2"))
    {
      ++m_MS_data.ms_data[index].nClass[14];
    }
    else if (!std::strcmp(szClass, "BFS1"))
    {
      ++m_MS_data.ms_data[index].nClass[15];
    }
    else if (!std::strcmp(szClass, "BFS2"))
    {
      ++m_MS_data.ms_data[index].nClass[16];
    }
    else if (!std::strcmp(szClass, "BFS3"))
    {
      ++m_MS_data.ms_data[index].nClass[17];
    }
    else if (!std::strcmp(szClass, "BSB0"))
    {
      ++m_MS_data.ms_data[index].nClass[18];
    }
    else if (!std::strcmp(szClass, "BSF1"))
    {
      ++m_MS_data.ms_data[index].nClass[19];
    }
    else if (!std::strcmp(szClass, "BSF2"))
    {
      ++m_MS_data.ms_data[index].nClass[20];
    }
    else if (!std::strcmp(szClass, "BSS1"))
    {
      ++m_MS_data.ms_data[index].nClass[21];
    }
    else if (!std::strcmp(szClass, "BSS2"))
    {
      ++m_MS_data.ms_data[index].nClass[22];
    }
    else if (!std::strcmp(szClass, "CWB0"))
    {
      ++m_MS_data.ms_data[index].nClass[23];
    }
    else if (!std::strcmp(szClass, "CWF1"))
    {
      ++m_MS_data.ms_data[index].nClass[24];
    }
    else if (!std::strcmp(szClass, "CWF2"))
    {
      ++m_MS_data.ms_data[index].nClass[25];
    }
    else if (!std::strcmp(szClass, "CWS1"))
    {
      ++m_MS_data.ms_data[index].nClass[26];
    }
    else if (!std::strcmp(szClass, "CWS2"))
    {
      ++m_MS_data.ms_data[index].nClass[27];
    }
    else if (!std::strcmp(szClass, "CWS3"))
    {
      ++m_MS_data.ms_data[index].nClass[28];
    }
    else if (!std::strcmp(szClass, "CRB0"))
    {
      ++m_MS_data.ms_data[index].nClass[29];
    }
    else if (!std::strcmp(szClass, "CRF1"))
    {
      ++m_MS_data.ms_data[index].nClass[30];
    }
    else if (!std::strcmp(szClass, "CRF2"))
    {
      ++m_MS_data.ms_data[index].nClass[31];
    }
    else if (!std::strcmp(szClass, "CRS1"))
    {
      ++m_MS_data.ms_data[index].nClass[32];
    }
    else if (!std::strcmp(szClass, "CRS2"))
    {
      ++m_MS_data.ms_data[index].nClass[33];
    }
    else if (!std::strcmp(szClass, "CRS3"))
    {
      ++m_MS_data.ms_data[index].nClass[34];
    }
    else if (!std::strcmp(szClass, "CFB0"))
    {
      ++m_MS_data.ms_data[index].nClass[35];
    }
    else if (!std::strcmp(szClass, "CFF1"))
    {
      ++m_MS_data.ms_data[index].nClass[36];
    }
    else if (!std::strcmp(szClass, "CFF2"))
    {
      ++m_MS_data.ms_data[index].nClass[37];
    }
    else if (!std::strcmp(szClass, "CFS1"))
    {
      ++m_MS_data.ms_data[index].nClass[38];
    }
    else if (!std::strcmp(szClass, "CFS2"))
    {
      ++m_MS_data.ms_data[index].nClass[39];
    }
    else if (!std::strcmp(szClass, "CFS3"))
    {
      ++m_MS_data.ms_data[index].nClass[40];
    }
    else if (!std::strcmp(szClass, "CSB0"))
    {
      ++m_MS_data.ms_data[index].nClass[41];
    }
    else if (!std::strcmp(szClass, "CSF1"))
    {
      ++m_MS_data.ms_data[index].nClass[42];
    }
    else if (!std::strcmp(szClass, "CSS1"))
    {
      ++m_MS_data.ms_data[index].nClass[43];
    }
    else if (!std::strcmp(szClass, "AWB0"))
    {
      ++m_MS_data.ms_data[index].nClass[44];
    }
    else if (!std::strcmp(szClass, "AWF1"))
    {
      ++m_MS_data.ms_data[index].nClass[45];
    }
    else if (!std::strcmp(szClass, "AWF2"))
    {
      ++m_MS_data.ms_data[index].nClass[46];
    }
    else if (!std::strcmp(szClass, "AWS1"))
    {
      ++m_MS_data.ms_data[index].nClass[47];
    }
    else if (!std::strcmp(szClass, "AWS2"))
    {
      ++m_MS_data.ms_data[index].nClass[48];
    }
    else if (!std::strcmp(szClass, "AWS3"))
    {
      ++m_MS_data.ms_data[index].nClass[49];
    }
    else if (!std::strcmp(szClass, "ARB0"))
    {
      ++m_MS_data.ms_data[index].nClass[50];
    }
    else if (!std::strcmp(szClass, "ARF1"))
    {
      ++m_MS_data.ms_data[index].nClass[51];
    }
    else if (!std::strcmp(szClass, "ARF2"))
    {
      ++m_MS_data.ms_data[index].nClass[52];
    }
    else if (!std::strcmp(szClass, "ARS1"))
    {
      ++m_MS_data.ms_data[index].nClass[53];
    }
    else if (!std::strcmp(szClass, "ARS2"))
    {
      ++m_MS_data.ms_data[index].nClass[54];
    }
    else if (!std::strcmp(szClass, "ARS3"))
    {
      ++m_MS_data.ms_data[index].nClass[55];
    }
    else if (!std::strcmp(szClass, "ASB0"))
    {
      ++m_MS_data.ms_data[index].nClass[56];
    }
    else if (!std::strcmp(szClass, "ASF1"))
    {
      ++m_MS_data.ms_data[index].nClass[57];
    }
    else if (!std::strcmp(szClass, "ASS1"))
    {
      ++m_MS_data.ms_data[index].nClass[58];
    }
    else if (!std::strcmp(szClass, "ASS2"))
    {
      ++m_MS_data.ms_data[index].nClass[59];
    }
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

  if (!std::strcmp(szClass, "BWB0"))
    {
      ++m_MS_data.ms_data[0].nClass[0];
    }
    else if (!std::strcmp(szClass, "BWF1"))
    {
      ++m_MS_data.ms_data[0].nClass[1];
    }
    else if (!std::strcmp(szClass, "BWF2"))
    {
      ++m_MS_data.ms_data[0].nClass[2];
    }
    else if (!std::strcmp(szClass, "BWS1"))
    {
      ++m_MS_data.ms_data[0].nClass[3];
    }
    else if (!std::strcmp(szClass, "BWS2"))
    {
      ++m_MS_data.ms_data[0].nClass[4];
    }
    else if (!std::strcmp(szClass, "BWS3"))
    {
      ++m_MS_data.ms_data[0].nClass[5];
    }
    else if (!std::strcmp(szClass, "BRB0"))
    {
      ++m_MS_data.ms_data[0].nClass[6];
    }
    else if (!std::strcmp(szClass, "BRF1"))
    {
      ++m_MS_data.ms_data[0].nClass[7];
    }
    else if (!std::strcmp(szClass, "BRF2"))
    {
      ++m_MS_data.ms_data[0].nClass[8];
    }
    else if (!std::strcmp(szClass, "BRS1"))
    {
      ++m_MS_data.ms_data[0].nClass[9];
    }
    else if (!std::strcmp(szClass, "BRS2"))
    {
      ++m_MS_data.ms_data[0].nClass[10];
    }
    else if (!std::strcmp(szClass, "BRS3"))
    {
      ++m_MS_data.ms_data[0].nClass[11];
    }
    else if (!std::strcmp(szClass, "BFB0"))
    {
      ++m_MS_data.ms_data[0].nClass[12];
    }
    else if (!std::strcmp(szClass, "BFF1"))
    {
      ++m_MS_data.ms_data[0].nClass[13];
    }
    else if (!std::strcmp(szClass, "BFF2"))
    {
      ++m_MS_data.ms_data[0].nClass[14];
    }
    else if (!std::strcmp(szClass, "BFS1"))
    {
      ++m_MS_data.ms_data[0].nClass[15];
    }
    else if (!std::strcmp(szClass, "BFS2"))
    {
      ++m_MS_data.ms_data[0].nClass[16];
    }
    else if (!std::strcmp(szClass, "BFS3"))
    {
      ++m_MS_data.ms_data[0].nClass[17];
    }
    else if (!std::strcmp(szClass, "BSB0"))
    {
      ++m_MS_data.ms_data[0].nClass[18];
    }
    else if (!std::strcmp(szClass, "BSF1"))
    {
      ++m_MS_data.ms_data[0].nClass[19];
    }
    else if (!std::strcmp(szClass, "BSF2"))
    {
      ++m_MS_data.ms_data[0].nClass[20];
    }
    else if (!std::strcmp(szClass, "BSS1"))
    {
      ++m_MS_data.ms_data[0].nClass[21];
    }
    else if (!std::strcmp(szClass, "BSS2"))
    {
      ++m_MS_data.ms_data[0].nClass[22];
    }
    else if (!std::strcmp(szClass, "CWB0"))
    {
      ++m_MS_data.ms_data[0].nClass[23];
    }
    else if (!std::strcmp(szClass, "CWF1"))
    {
      ++m_MS_data.ms_data[0].nClass[24];
    }
    else if (!std::strcmp(szClass, "CWF2"))
    {
      ++m_MS_data.ms_data[0].nClass[25];
    }
    else if (!std::strcmp(szClass, "CWS1"))
    {
      ++m_MS_data.ms_data[0].nClass[26];
    }
    else if (!std::strcmp(szClass, "CWS2"))
    {
      ++m_MS_data.ms_data[0].nClass[27];
    }
    else if (!std::strcmp(szClass, "CWS3"))
    {
      ++m_MS_data.ms_data[0].nClass[28];
    }
    else if (!std::strcmp(szClass, "CRB0"))
    {
      ++m_MS_data.ms_data[0].nClass[29];
    }
    else if (!std::strcmp(szClass, "CRF1"))
    {
      ++m_MS_data.ms_data[0].nClass[30];
    }
    else if (!std::strcmp(szClass, "CRF2"))
    {
      ++m_MS_data.ms_data[0].nClass[31];
    }
    else if (!std::strcmp(szClass, "CRS1"))
    {
      ++m_MS_data.ms_data[0].nClass[32];
    }
    else if (!std::strcmp(szClass, "CRS2"))
    {
      ++m_MS_data.ms_data[0].nClass[33];
    }
    else if (!std::strcmp(szClass, "CRS3"))
    {
      ++m_MS_data.ms_data[0].nClass[34];
    }
    else if (!std::strcmp(szClass, "CFB0"))
    {
      ++m_MS_data.ms_data[0].nClass[35];
    }
    else if (!std::strcmp(szClass, "CFF1"))
    {
      ++m_MS_data.ms_data[0].nClass[36];
    }
    else if (!std::strcmp(szClass, "CFF2"))
    {
      ++m_MS_data.ms_data[0].nClass[37];
    }
    else if (!std::strcmp(szClass, "CFS1"))
    {
      ++m_MS_data.ms_data[0].nClass[38];
    }
    else if (!std::strcmp(szClass, "CFS2"))
    {
      ++m_MS_data.ms_data[0].nClass[39];
    }
    else if (!std::strcmp(szClass, "CFS3"))
    {
      ++m_MS_data.ms_data[0].nClass[40];
    }
    else if (!std::strcmp(szClass, "CSB0"))
    {
      ++m_MS_data.ms_data[0].nClass[41];
    }
    else if (!std::strcmp(szClass, "CSF1"))
    {
      ++m_MS_data.ms_data[0].nClass[42];
    }
    else if (!std::strcmp(szClass, "CSS1"))
    {
      ++m_MS_data.ms_data[0].nClass[43];
    }
    else if (!std::strcmp(szClass, "AWB0"))
    {
      ++m_MS_data.ms_data[0].nClass[44];
    }
    else if (!std::strcmp(szClass, "AWF1"))
    {
      ++m_MS_data.ms_data[0].nClass[45];
    }
    else if (!std::strcmp(szClass, "AWF2"))
    {
      ++m_MS_data.ms_data[0].nClass[46];
    }
    else if (!std::strcmp(szClass, "AWS1"))
    {
      ++m_MS_data.ms_data[0].nClass[47];
    }
    else if (!std::strcmp(szClass, "AWS2"))
    {
      ++m_MS_data.ms_data[0].nClass[48];
    }
    else if (!std::strcmp(szClass, "AWS3"))
    {
      ++m_MS_data.ms_data[0].nClass[49];
    }
    else if (!std::strcmp(szClass, "ARB0"))
    {
      ++m_MS_data.ms_data[0].nClass[50];
    }
    else if (!std::strcmp(szClass, "ARF1"))
    {
      ++m_MS_data.ms_data[0].nClass[51];
    }
    else if (!std::strcmp(szClass, "ARF2"))
    {
      ++m_MS_data.ms_data[0].nClass[52];
    }
    else if (!std::strcmp(szClass, "ARS1"))
    {
      ++m_MS_data.ms_data[0].nClass[53];
    }
    else if (!std::strcmp(szClass, "ARS2"))
    {
      ++m_MS_data.ms_data[0].nClass[54];
    }
    else if (!std::strcmp(szClass, "ARS3"))
    {
      ++m_MS_data.ms_data[0].nClass[55];
    }
    else if (!std::strcmp(szClass, "ASB0"))
    {
      ++m_MS_data.ms_data[0].nClass[56];
    }
    else if (!std::strcmp(szClass, "ASF1"))
    {
      ++m_MS_data.ms_data[0].nClass[57];
    }
    else if (!std::strcmp(szClass, "ASS1"))
    {
      ++m_MS_data.ms_data[0].nClass[58];
    }
    else if (!std::strcmp(szClass, "ASS2"))
    {
      ++m_MS_data.ms_data[0].nClass[59];
    }
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

  if (!std::strcmp(szClass, "BWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[0];
  }
  else if (!std::strcmp(szClass, "BWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[4];
  }
  else if (!std::strcmp(szClass, "BWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[5];
  }
  else if (!std::strcmp(szClass, "BRB0"))
  {
    ++m_MS_data.ms_data[1].nClass[6];
  }
  else if (!std::strcmp(szClass, "BRF1"))
  {
    ++m_MS_data.ms_data[1].nClass[7];
  }
  else if (!std::strcmp(szClass, "BRF2"))
  {
    ++m_MS_data.ms_data[1].nClass[8];
  }
  else if (!std::strcmp(szClass, "BRS1"))
  {
    ++m_MS_data.ms_data[1].nClass[9];
  }
  else if (!std::strcmp(szClass, "BRS2"))
  {
    ++m_MS_data.ms_data[1].nClass[10];
  }
  else if (!std::strcmp(szClass, "BRS3"))
  {
    ++m_MS_data.ms_data[1].nClass[11];
  }
  else if (!std::strcmp(szClass, "BFB0"))
  {
    ++m_MS_data.ms_data[1].nClass[12];
  }
  else if (!std::strcmp(szClass, "BFF1"))
  {
    ++m_MS_data.ms_data[1].nClass[13];
  }
  else if (!std::strcmp(szClass, "BFF2"))
  {
    ++m_MS_data.ms_data[1].nClass[14];
  }
  else if (!std::strcmp(szClass, "BFS1"))
  {
    ++m_MS_data.ms_data[1].nClass[15];
  }
  else if (!std::strcmp(szClass, "BFS2"))
  {
    ++m_MS_data.ms_data[1].nClass[16];
  }
  else if (!std::strcmp(szClass, "BFS3"))
  {
    ++m_MS_data.ms_data[1].nClass[17];
  }
  else if (!std::strcmp(szClass, "BSB0"))
  {
    ++m_MS_data.ms_data[1].nClass[18];
  }
  else if (!std::strcmp(szClass, "BSF1"))
  {
    ++m_MS_data.ms_data[1].nClass[19];
  }
  else if (!std::strcmp(szClass, "BSF2"))
  {
    ++m_MS_data.ms_data[1].nClass[20];
  }
  else if (!std::strcmp(szClass, "BSS1"))
  {
    ++m_MS_data.ms_data[1].nClass[21];
  }
  else if (!std::strcmp(szClass, "BSS2"))
  {
    ++m_MS_data.ms_data[1].nClass[22];
  }
  else if (!std::strcmp(szClass, "CWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[23];
  }
  else if (!std::strcmp(szClass, "CWF1"))
  {
    ++m_MS_data.ms_data[1].nClass[24];
  }
  else if (!std::strcmp(szClass, "CWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[25];
  }
  else if (!std::strcmp(szClass, "CWS1"))
  {
    ++m_MS_data.ms_data[1].nClass[26];
  }
  else if (!std::strcmp(szClass, "CWS2"))
  {
    ++m_MS_data.ms_data[1].nClass[27];
  }
  else if (!std::strcmp(szClass, "CWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[28];
  }
  else if (!std::strcmp(szClass, "CRB0"))
  {
    ++m_MS_data.ms_data[1].nClass[29];
  }
  else if (!std::strcmp(szClass, "CRF1"))
  {
    ++m_MS_data.ms_data[1].nClass[30];
  }
  else if (!std::strcmp(szClass, "CRF2"))
  {
    ++m_MS_data.ms_data[1].nClass[31];
  }
  else if (!std::strcmp(szClass, "CRS1"))
  {
    ++m_MS_data.ms_data[1].nClass[32];
  }
  else if (!std::strcmp(szClass, "CRS2"))
  {
    ++m_MS_data.ms_data[1].nClass[33];
  }
  else if (!std::strcmp(szClass, "CRS3"))
  {
    ++m_MS_data.ms_data[1].nClass[34];
  }
  else if (!std::strcmp(szClass, "CFB0"))
  {
    ++m_MS_data.ms_data[1].nClass[35];
  }
  else if (!std::strcmp(szClass, "CFF1"))
  {
    ++m_MS_data.ms_data[1].nClass[36];
  }
  else if (!std::strcmp(szClass, "CFF2"))
  {
    ++m_MS_data.ms_data[1].nClass[37];
  }
  else if (!std::strcmp(szClass, "CFS1"))
  {
    ++m_MS_data.ms_data[1].nClass[38];
  }
  else if (!std::strcmp(szClass, "CFS2"))
  {
    ++m_MS_data.ms_data[1].nClass[39];
  }
  else if (!std::strcmp(szClass, "CFS3"))
  {
    ++m_MS_data.ms_data[1].nClass[40];
  }
  else if (!std::strcmp(szClass, "CSB0"))
  {
    ++m_MS_data.ms_data[1].nClass[41];
  }
  else if (!std::strcmp(szClass, "CSF1"))
  {
    ++m_MS_data.ms_data[1].nClass[42];
  }
  else if (!std::strcmp(szClass, "CSS1"))
  {
    ++m_MS_data.ms_data[1].nClass[43];
  }
  else if (!std::strcmp(szClass, "AWB0"))
  {
    ++m_MS_data.ms_data[1].nClass[44];
  }
  else if (!std::strcmp(szClass, "AWF1"))
  {
    ++m_MS_data.ms_data[1].nClass[45];
  }
  else if (!std::strcmp(szClass, "AWF2"))
  {
    ++m_MS_data.ms_data[1].nClass[46];
  }
  else if (!std::strcmp(szClass, "AWS1"))
  {
    ++m_MS_data.ms_data[1].nClass[47];
  }
  else if (!std::strcmp(szClass, "AWS2"))
  {
    ++m_MS_data.ms_data[1].nClass[48];
  }
  else if (!std::strcmp(szClass, "AWS3"))
  {
    ++m_MS_data.ms_data[1].nClass[49];
  }
  else if (!std::strcmp(szClass, "ARB0"))
  {
    ++m_MS_data.ms_data[1].nClass[50];
  }
  else if (!std::strcmp(szClass, "ARF1"))
  {
    ++m_MS_data.ms_data[1].nClass[51];
  }
  else if (!std::strcmp(szClass, "ARF2"))
  {
    ++m_MS_data.ms_data[1].nClass[52];
  }
  else if (!std::strcmp(szClass, "ARS1"))
  {
    ++m_MS_data.ms_data[1].nClass[53];
  }
  else if (!std::strcmp(szClass, "ARS2"))
  {
    ++m_MS_data.ms_data[1].nClass[54];
  }
  else if (!std::strcmp(szClass, "ARS3"))
  {
    ++m_MS_data.ms_data[1].nClass[55];
  }
  else if (!std::strcmp(szClass, "ASB0"))
  {
    ++m_MS_data.ms_data[1].nClass[56];
  }
  else if (!std::strcmp(szClass, "ASF1"))
  {
    ++m_MS_data.ms_data[1].nClass[57];
  }
  else if (!std::strcmp(szClass, "ASS1"))
  {
    ++m_MS_data.ms_data[1].nClass[58];
  }
  else if (!std::strcmp(szClass, "ASS2"))
  {
    ++m_MS_data.ms_data[1].nClass[59];
  }
}

void CMoneySupplyMgr::UpdateGateRewardMoneyData(unsigned __int8 byRace, int nLv, char *szClass, unsigned int nAmount)
{
  m_MS_data.dwAmount[2] += nAmount;
  switch (nLv)
  {
    case 30:
      ++m_MS_data.ms_data[2].nLv[0];
      break;
    case 40:
      ++m_MS_data.ms_data[2].nLv[1];
      break;
    case 50:
      ++m_MS_data.ms_data[2].nLv[2];
      break;
    case 60:
      ++m_MS_data.ms_data[2].nLv[3];
      break;
    default:
      break;
  }

  if (!byRace)
  {
    ++m_MS_data.ms_data[2].nRace[0];
  }
  if (byRace == 1)
  {
    ++m_MS_data.ms_data[2].nRace[1];
  }
  else
  {
    ++m_MS_data.ms_data[2].nRace[2];
  }

  if (!std::strcmp(szClass, "BWB0"))
  {
    ++m_MS_data.ms_data[2].nClass[0];
  }
  else if (!std::strcmp(szClass, "BWF2"))
  {
    ++m_MS_data.ms_data[2].nClass[4];
  }
  else if (!std::strcmp(szClass, "BWS3"))
  {
    ++m_MS_data.ms_data[2].nClass[5];
  }
  else if (!std::strcmp(szClass, "BRB0"))
  {
    ++m_MS_data.ms_data[2].nClass[6];
  }
  else if (!std::strcmp(szClass, "BRF1"))
  {
    ++m_MS_data.ms_data[2].nClass[7];
  }
  else if (!std::strcmp(szClass, "BRF2"))
  {
    ++m_MS_data.ms_data[2].nClass[8];
  }
  else if (!std::strcmp(szClass, "BRS1"))
  {
    ++m_MS_data.ms_data[2].nClass[9];
  }
  else if (!std::strcmp(szClass, "BRS2"))
  {
    ++m_MS_data.ms_data[2].nClass[10];
  }
  else if (!std::strcmp(szClass, "BRS3"))
  {
    ++m_MS_data.ms_data[2].nClass[11];
  }
  else if (!std::strcmp(szClass, "BFB0"))
  {
    ++m_MS_data.ms_data[2].nClass[12];
  }
  else if (!std::strcmp(szClass, "BFF1"))
  {
    ++m_MS_data.ms_data[2].nClass[13];
  }
  else if (!std::strcmp(szClass, "BFF2"))
  {
    ++m_MS_data.ms_data[2].nClass[14];
  }
  else if (!std::strcmp(szClass, "BFS1"))
  {
    ++m_MS_data.ms_data[2].nClass[15];
  }
  else if (!std::strcmp(szClass, "BFS2"))
  {
    ++m_MS_data.ms_data[2].nClass[16];
  }
  else if (!std::strcmp(szClass, "BFS3"))
  {
    ++m_MS_data.ms_data[2].nClass[17];
  }
  else if (!std::strcmp(szClass, "BSB0"))
  {
    ++m_MS_data.ms_data[2].nClass[18];
  }
  else if (!std::strcmp(szClass, "BSF1"))
  {
    ++m_MS_data.ms_data[2].nClass[19];
  }
  else if (!std::strcmp(szClass, "BSF2"))
  {
    ++m_MS_data.ms_data[2].nClass[20];
  }
  else if (!std::strcmp(szClass, "BSS1"))
  {
    ++m_MS_data.ms_data[2].nClass[21];
  }
  else if (!std::strcmp(szClass, "BSS2"))
  {
    ++m_MS_data.ms_data[2].nClass[22];
  }
  else if (!std::strcmp(szClass, "CWB0"))
  {
    ++m_MS_data.ms_data[2].nClass[23];
  }
  else if (!std::strcmp(szClass, "CWF1"))
  {
    ++m_MS_data.ms_data[2].nClass[24];
  }
  else if (!std::strcmp(szClass, "CWF2"))
  {
    ++m_MS_data.ms_data[2].nClass[25];
  }
  else if (!std::strcmp(szClass, "CWS1"))
  {
    ++m_MS_data.ms_data[2].nClass[26];
  }
  else if (!std::strcmp(szClass, "CWS2"))
  {
    ++m_MS_data.ms_data[2].nClass[27];
  }
  else if (!std::strcmp(szClass, "CWS3"))
  {
    ++m_MS_data.ms_data[2].nClass[28];
  }
  else if (!std::strcmp(szClass, "CRB0"))
  {
    ++m_MS_data.ms_data[2].nClass[29];
  }
  else if (!std::strcmp(szClass, "CRF1"))
  {
    ++m_MS_data.ms_data[2].nClass[30];
  }
  else if (!std::strcmp(szClass, "CRF2"))
  {
    ++m_MS_data.ms_data[2].nClass[31];
  }
  else if (!std::strcmp(szClass, "CRS1"))
  {
    ++m_MS_data.ms_data[2].nClass[32];
  }
  else if (!std::strcmp(szClass, "CRS2"))
  {
    ++m_MS_data.ms_data[2].nClass[33];
  }
  else if (!std::strcmp(szClass, "CRS3"))
  {
    ++m_MS_data.ms_data[2].nClass[34];
  }
  else if (!std::strcmp(szClass, "CFB0"))
  {
    ++m_MS_data.ms_data[2].nClass[35];
  }
  else if (!std::strcmp(szClass, "CFF1"))
  {
    ++m_MS_data.ms_data[2].nClass[36];
  }
  else if (!std::strcmp(szClass, "CFF2"))
  {
    ++m_MS_data.ms_data[2].nClass[37];
  }
  else if (!std::strcmp(szClass, "CFS1"))
  {
    ++m_MS_data.ms_data[2].nClass[38];
  }
  else if (!std::strcmp(szClass, "CFS2"))
  {
    ++m_MS_data.ms_data[2].nClass[39];
  }
  else if (!std::strcmp(szClass, "CFS3"))
  {
    ++m_MS_data.ms_data[2].nClass[40];
  }
  else if (!std::strcmp(szClass, "CSB0"))
  {
    ++m_MS_data.ms_data[2].nClass[41];
  }
  else if (!std::strcmp(szClass, "CSF1"))
  {
    ++m_MS_data.ms_data[2].nClass[42];
  }
  else if (!std::strcmp(szClass, "CSS1"))
  {
    ++m_MS_data.ms_data[2].nClass[43];
  }
  else if (!std::strcmp(szClass, "AWB0"))
  {
    ++m_MS_data.ms_data[2].nClass[44];
  }
  else if (!std::strcmp(szClass, "AWF1"))
  {
    ++m_MS_data.ms_data[2].nClass[45];
  }
  else if (!std::strcmp(szClass, "AWF2"))
  {
    ++m_MS_data.ms_data[2].nClass[46];
  }
  else if (!std::strcmp(szClass, "AWS1"))
  {
    ++m_MS_data.ms_data[2].nClass[47];
  }
  else if (!std::strcmp(szClass, "AWS2"))
  {
    ++m_MS_data.ms_data[2].nClass[48];
  }
  else if (!std::strcmp(szClass, "AWS3"))
  {
    ++m_MS_data.ms_data[2].nClass[49];
  }
  else if (!std::strcmp(szClass, "ARB0"))
  {
    ++m_MS_data.ms_data[2].nClass[50];
  }
  else if (!std::strcmp(szClass, "ARF1"))
  {
    ++m_MS_data.ms_data[2].nClass[51];
  }
  else if (!std::strcmp(szClass, "ARF2"))
  {
    ++m_MS_data.ms_data[2].nClass[52];
  }
  else if (!std::strcmp(szClass, "ARS1"))
  {
    ++m_MS_data.ms_data[2].nClass[53];
  }
  else if (!std::strcmp(szClass, "ARS2"))
  {
    ++m_MS_data.ms_data[2].nClass[54];
  }
  else if (!std::strcmp(szClass, "ARS3"))
  {
    ++m_MS_data.ms_data[2].nClass[55];
  }
  else if (!std::strcmp(szClass, "ASB0"))
  {
    ++m_MS_data.ms_data[2].nClass[56];
  }
  else if (!std::strcmp(szClass, "ASF1"))
  {
    ++m_MS_data.ms_data[2].nClass[57];
  }
  else if (!std::strcmp(szClass, "ASS1"))
  {
    ++m_MS_data.ms_data[2].nClass[58];
  }
  else if (!std::strcmp(szClass, "ASS2"))
  {
    ++m_MS_data.ms_data[2].nClass[59];
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


