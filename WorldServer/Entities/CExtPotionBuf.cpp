#include "pch.h"

#include "CExtPotionBuf.h"

#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPotionMgr.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

CExtPotionBuf::CExtPotionBuf()
  : m_bExtPotionBufUse(false), m_bDayChange(false), m_dwEndPotionTime(0)
{
}

CExtPotionBuf::~CExtPotionBuf()
{
  // this is not a stub
}

void CExtPotionBuf::SetExtPotionBufUse(bool bUse)
{
  m_bExtPotionBufUse = bUse;
}

void CExtPotionBuf::SetExtPotionEndTime(unsigned int dwEndTime)
{
  m_dwEndPotionTime = dwEndTime;
}

void CExtPotionBuf::CalcRemainTime(unsigned __int16 wInx, bool bUse)
{
  int nEndDay = 0;
  int nEndHour = 0;
  int nEndMin = 0;

  const unsigned int year = m_dwEndPotionTime / 1000000;
  const unsigned int month = (m_dwEndPotionTime % 1000000) / 10000;
  const unsigned int day = (m_dwEndPotionTime % 10000) / 100;
  const unsigned int minute = m_dwEndPotionTime % 100;

  if (year <= GetKorLocalTime() / 1000000)
  {
    nEndDay = static_cast<int>(month) - static_cast<int>(GetCurrentDay());
  }
  else
  {
    int monthDays = 31;
    if (GetCurrentMonth() == 2)
    {
      if ((GetCurrentYear() % 4 || !(GetCurrentYear() % 100)) && GetCurrentYear() % 400)
      {
        monthDays = 28;
      }
      else
      {
        monthDays = 29;
      }
    }
    else if (GetCurrentMonth() == 4 || GetCurrentMonth() == 6 || GetCurrentMonth() == 9 || GetCurrentMonth() == 11)
    {
      monthDays = 30;
    }

    nEndDay = static_cast<int>(monthDays + month - GetCurrentDay());
  }

  if (day <= GetCurrentHour())
  {
    if (nEndDay || day != GetCurrentHour())
    {
      nEndHour = static_cast<int>(day + 24 - GetCurrentHour());
    }
    else
    {
      nEndHour = 0;
    }
  }
  else
  {
    nEndHour = static_cast<int>(day - GetCurrentHour());
  }

  if (minute <= GetCurrentMin())
  {
    if (nEndDay || nEndHour || minute != GetCurrentMin())
    {
      nEndMin = static_cast<int>(minute + 60 - GetCurrentMin());
    }
    else
    {
      nEndMin = 0;
    }
  }
  else
  {
    nEndMin = static_cast<int>(minute - GetCurrentMin());
  }

  SendMsg_RemainBufUseTime(
    static_cast<char>(bUse),
    wInx,
    static_cast<char>(nEndDay),
    static_cast<char>(nEndHour),
    static_cast<char>(nEndMin));
}

bool CExtPotionBuf::IsExtPotionUse()
{
  return m_bExtPotionBufUse;
}

void CExtPotionBuf::CheckPotionTime(CPlayer *pOne)
{
  if (m_dwEndPotionTime <= GetKorLocalTime())
  {
    m_bExtPotionBufUse = false;
    pOne->m_pUserDB->m_AvatorData.dbSupplement.dwBufPotionEndTime = 0;
    SendMsg_RemainBufUseTime(0, pOne->m_ObjID.m_wIndex, 0, 0, 0);
    g_PotionMgr.SelectDeleteBuf(pOne, true, true);
  }

  if (!m_bDayChange && !GetCurrentHour())
  {
    m_bDayChange = true;
    CalcRemainTime(pOne->m_ObjID.m_wIndex, true);
  }

  if (m_bDayChange && GetCurrentHour() > 0)
  {
    m_bDayChange = false;
  }
}

void CExtPotionBuf::SendMsg_RemainBufUseTime(
  char bUse,
  unsigned __int16 wIndex,
  char nEndDay,
  char nEndHour,
  char nEndMin)
{
  char msg[4]{bUse, nEndDay, nEndHour, nEndMin};
  unsigned __int8 pbyType[2]{7, 110};
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, pbyType, msg, 4u);
}

void CExtPotionBuf::SednMsg_RemovePotionContEffect(unsigned __int16 wPotionInx, unsigned __int16 wIndex)
{
  char msg[2]{};
  *reinterpret_cast<unsigned __int16 *>(msg) = wPotionInx;
  unsigned __int8 pbyType[2]{7, 109};
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, pbyType, msg, 2u);
}

void CExtPotionBuf::UseBuffPotion(CPlayer *pOne)
{
  m_bExtPotionBufUse = true;
  m_dwEndPotionTime = GetConnectTime_AddBySec(2592000);
  pOne->m_pUserDB->m_AvatorData.dbSupplement.dwBufPotionEndTime = m_dwEndPotionTime;
  SendMsg_RemainBufUseTime(1, pOne->m_ObjID.m_wIndex, 30, 0, 0);
}

