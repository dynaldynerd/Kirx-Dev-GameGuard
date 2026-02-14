#include "pch.h"

#include "COreAmountMgr.h"

#include <cstdio>
#include <cstring>
#include <new>

#include "CHolyStoneSystem.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "qry_case_insert_orelog.h"

COreAmountMgr *COreAmountMgr::m_pInstance = nullptr;

COreAmountMgr *COreAmountMgr::Instance()
{
  if (!m_pInstance)
  {
    m_pInstance = new (std::nothrow) COreAmountMgr();
  }
  return m_pInstance;
}

COreAmountMgr::COreAmountMgr()
{
  m_dwTotalOreSet = 0;
  m_dwMinOreSet = 0;
  m_dwTotalOreAmount = 0;
  m_dwRemainOreAmount = 0;
  m_uiProccessIndex = 0;
  m_byDepositRate = 0;
  m_bChangeRemainRate = 0;
  std::memset(m_fMultipleRate, 0, sizeof(m_fMultipleRate));
  m_ftWrite.dwLowDateTime = 0;
  m_ftWrite.dwHighDateTime = 0;

  const unsigned int korLocalTime = GetKorLocalTime();
  char logPath[128]{};
  sprintf_s(logPath, "..\\ZoneServerLog\\ServiceLog\\OreAmount_%d.log", korLocalTime);
  m_logOreAmount.SetWriteLogFile(logPath, 1, 0, 1, 1);

  m_byOreTransferCount = 0;
  m_dwOreTransferAmount = 0;
  m_bCheckExhOreLog = false;
}

COreAmountMgr::~COreAmountMgr() = default;

void COreAmountMgr::Release()
{
  if (m_pInstance)
  {
    delete m_pInstance;
    m_pInstance = nullptr;
  }
}

bool COreAmountMgr::LoadINI()
{
  _FILETIME writeTime{};
  if (!GetLastWriteFileTime(".\\Initialize\\NewHolySystem.ini", &writeTime))
  {
    g_Main.m_logLoadingError.Write(
      "COreAmountMgr::Init Not Exist %s File!",
      ".\\Initialize\\NewHolySystem.ini");
    return false;
  }

  m_ftWrite = writeTime;
  m_dwTotalOreSet = GetPrivateProfileIntA("OreTotalAmount", "Set", 0, ".\\Initialize\\NewHolySystem.ini");
  if (!m_dwTotalOreSet)
  {
    return false;
  }

  m_dwMinOreSet = GetPrivateProfileIntA("OreTotalAmount", "MinSet", 0, ".\\Initialize\\NewHolySystem.ini");
  if (!m_dwMinOreSet)
  {
    return false;
  }

  for (int dayIndex = 0; dayIndex < 7; ++dayIndex)
  {
    for (int timeIndex = 0; timeIndex < 3; ++timeIndex)
    {
      char key[68]{};
      sprintf_s(key, "%s_%d", dayofweek[dayIndex], timeIndex + 1);
      const int rate =
        GetPrivateProfileIntA("OreTotalAmount", key, 100, ".\\Initialize\\NewHolySystem.ini");
      m_fMultipleRate[dayIndex][timeIndex] = static_cast<float>(rate);
      if (m_fMultipleRate[dayIndex][timeIndex] == 0.0f)
      {
        return false;
      }
      m_fMultipleRate[dayIndex][timeIndex] = m_fMultipleRate[dayIndex][timeIndex] / 100.0f;
    }
  }
  return true;
}

bool COreAmountMgr::IsINIFileChanged()
{
  _FILETIME writeTime{};
  if (!GetLastWriteFileTime(".\\Initialize\\NewHolySystem.ini", &writeTime))
  {
    g_Main.m_logLoadingError.Write(
      "COreAmountMgr::IsINIFileChanged() Not Exist %s File!",
      ".\\Initialize\\NewHolySystem.ini");
    return false;
  }

  return m_ftWrite.dwHighDateTime != writeTime.dwHighDateTime
      || m_ftWrite.dwLowDateTime != writeTime.dwLowDateTime;
}

bool COreAmountMgr::ReLoad()
{
  if (!IsINIFileChanged())
  {
    return false;
  }
  if (!LoadINI())
  {
    return false;
  }

  m_logOreAmount.Write("COreAmountMgr::ReLoad() Success!");
  return true;
}

void COreAmountMgr::InitRemainOreAmount(unsigned int dwRemain, unsigned int dwTotal)
{
  m_logOreAmount.Write(
    "Pre InitRemainOreAmount Ore Amount >> Remain:%d, Total:%d",
    m_dwRemainOreAmount,
    m_dwTotalOreAmount);

  if (dwRemain == static_cast<unsigned int>(-1))
  {
    const int liveNum = CPlayer::s_nLiveNum;
    if (liveNum >= 200)
    {
      const unsigned int totalSet = 297 * m_dwTotalOreSet;
      m_dwTotalOreAmount = (liveNum / 10) * totalSet;
    }
    else
    {
      m_dwTotalOreAmount = 297 * m_dwMinOreSet;
    }

    const int dayIndex = GetCurwDay();
    const int timeIndex = g_HolySys.GetNumOfTime() - 1;
    if (m_fMultipleRate[dayIndex][timeIndex] != 1.0f)
    {
      m_logOreAmount.Write("Pre Apply Multiple Rate Ore Amount >> Total:%d", m_dwTotalOreAmount);
      m_dwTotalOreAmount = static_cast<unsigned int>(
        static_cast<float>(static_cast<int>(m_dwTotalOreAmount)) * m_fMultipleRate[dayIndex][timeIndex]);
      m_logOreAmount.Write("Post Apply Multiple Rate Ore Amount >> Total:%d", m_dwTotalOreAmount);
    }

    m_dwRemainOreAmount = m_dwTotalOreAmount;

    if (!g_HolySys.GetDestroyerState())
    {
      const unsigned int reduced = 70 * m_dwRemainOreAmount / 100;
      m_logOreAmount.Write(
        "Reduce Ore Amount [Touch Down Fail] >> Pre:%d, Post:%d",
        m_dwRemainOreAmount,
        reduced);
      m_dwRemainOreAmount = reduced;
    }
  }
  else
  {
    const unsigned int total = (dwTotal == static_cast<unsigned int>(-1)) ? 0 : dwTotal;
    m_dwTotalOreAmount = total;
    m_dwRemainOreAmount = dwRemain;
    if (m_dwRemainOreAmount > m_dwTotalOreAmount)
    {
      m_dwTotalOreAmount = m_dwRemainOreAmount;
    }
  }

  m_bCheckExhOreLog = true;
  m_logOreAmount.Write(
    "Post InitRemainOreAmount Ore Amount >> Remain:%d, Total:%d",
    m_dwRemainOreAmount,
    m_dwTotalOreAmount);
  UpdateDepositeRate();
}

void COreAmountMgr::InitTransferOre(int nTransfer, int nInit)
{
  if (nInit)
  {
    m_byOreTransferCount = static_cast<unsigned __int8>(nInit);
    m_dwOreTransferAmount = static_cast<unsigned int>(nTransfer);
  }
  else
  {
    m_byOreTransferCount = 0;
    m_dwOreTransferAmount = 0;
  }
  UpdateForce();
}

unsigned int COreAmountMgr::GetRemainOre()
{
  return m_dwRemainOreAmount;
}

unsigned int COreAmountMgr::GetTotalOre()
{
  return m_dwTotalOreAmount;
}

unsigned int COreAmountMgr::GetOreTransferAmount()
{
  return m_dwOreTransferAmount;
}

unsigned __int8 COreAmountMgr::GetOreTransferCount()
{
  return m_byOreTransferCount;
}

unsigned __int8 COreAmountMgr::GetDepositRate()
{
  return m_byDepositRate;
}

float *COreAmountMgr::GetMultipleRate()
{
  return m_fMultipleRate[0];
}

void COreAmountMgr::UpdateDepositeRate()
{
  if (!m_dwTotalOreAmount)
  {
    const unsigned int fallback = m_dwRemainOreAmount ? m_dwRemainOreAmount : 1;
    m_dwTotalOreAmount = fallback;
    m_logOreAmount.Write(
      "Total ore amount is zero >> Fixed total:%d, remain:%d",
      m_dwTotalOreAmount,
      m_dwRemainOreAmount);
  }

  unsigned __int8 newRate =
    static_cast<unsigned __int8>(100 * m_dwRemainOreAmount / m_dwTotalOreAmount);
  if (!newRate)
  {
    newRate = (m_dwRemainOreAmount != 0);
  }

  if (m_byDepositRate != newRate)
  {
    m_byDepositRate = newRate;
    if (g_Main.m_bWorldOpen)
    {
      m_bChangeRemainRate = 1;
    }
  }
}

void COreAmountMgr::UpdateForce()
{
  if (!m_dwTotalOreAmount)
  {
    const unsigned int fallback = m_dwRemainOreAmount ? m_dwRemainOreAmount : 1;
    m_dwTotalOreAmount = fallback;
    m_logOreAmount.Write(
      "Total ore amount is zero >> Fixed total:%d, remain:%d",
      m_dwTotalOreAmount,
      m_dwRemainOreAmount);
  }

  unsigned __int8 newRate =
    static_cast<unsigned __int8>(100 * m_dwRemainOreAmount / m_dwTotalOreAmount);
  if (!newRate)
  {
    newRate = (m_dwRemainOreAmount != 0);
  }

  m_byDepositRate = newRate;
  m_bChangeRemainRate = 1;
}

void COreAmountMgr::IncreaseOreAmount()
{
  if (m_byOreTransferCount >= 10)
  {
    InitRemainOreAmount(0xFFFFFFFF, 0xFFFFFFFF);
    SetOreTransferAmount();
  }
  else
  {
    m_dwOreTransferAmount += m_dwRemainOreAmount;
  }

  m_dwRemainOreAmount = 0;
  UpdateForce();
}

void COreAmountMgr::IncreaseOreCount()
{
  if (m_byOreTransferCount >= 10)
  {
    m_byOreTransferCount = 0;
  }
  else
  {
    ++m_byOreTransferCount;
  }
  UpdateForce();
}

void COreAmountMgr::DecreaseOre(unsigned int dwAlt)
{
  m_dwRemainOreAmount -= dwAlt;
  UpdateDepositeRate();
  if (!m_dwRemainOreAmount && m_bCheckExhOreLog)
  {
    InsertOreLog(0);
    m_bCheckExhOreLog = false;
  }
}

void COreAmountMgr::SetOreTransferAmount()
{
  m_dwOreTransferAmount = 0;
}

void COreAmountMgr::InsertOreLog(unsigned __int8 byType)
{
  if (!g_Main.m_bDQSThread)
  {
    return;
  }

  char zeroDate[17]{};
  const int liveUserCount = CPlayer::s_nLiveNum;
  _qry_case_insert_orelog qryData;
  char szDateTime[40]{};

  GetNowDateTime(szDateTime);
  qryData.byType = byType;
  std::memcpy(qryData.szLogDate, zeroDate, sizeof(qryData.szLogDate));
  qryData.nLiveUsercnt = liveUserCount;
  qryData.dwOreRemain = m_dwRemainOreAmount;
  qryData.dwTAmount = m_dwTotalOreAmount;

  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0xACu, reinterpret_cast<char *>(&qryData), 32);
}

void COreAmountMgr::Loop()
{
  if (m_bChangeRemainRate)
  {
    LoopSubProcSendInform();
  }
}

void COreAmountMgr::LoopSubProcSendInform()
{
  const unsigned int endIndex = m_uiProccessIndex + 50;
  unsigned int index = m_uiProccessIndex;
  for (; index < endIndex && index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bLive && player->m_bOper)
    {
      player->SendMsg_RemainOreRate();
      player->SendMsg_OreTransferCount();
    }
  }
  m_uiProccessIndex = index;
  if (m_uiProccessIndex >= MAX_PLAYER)
  {
    m_uiProccessIndex = 0;
    m_bChangeRemainRate = 0;
  }
}

bool COreAmountMgr::CheatOreAmount(unsigned int dwTot, unsigned int dwRemain)
{
  if (dwTot)
  {
    m_dwTotalOreAmount = dwTot;
  }

  if (dwRemain <= m_dwTotalOreAmount)
  {
    m_dwRemainOreAmount = dwRemain;
  }
  else
  {
    m_dwRemainOreAmount = m_dwTotalOreAmount;
  }

  DecreaseOre(0);
  return true;
}

bool COreAmountMgr::IsOreRemain()
{
  return m_dwRemainOreAmount != 0;
}
