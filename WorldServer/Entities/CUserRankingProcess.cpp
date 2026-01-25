#include "pch.h"

#include "CUserRankingProcess.h"

#include "WorldServerUtil.h"

#include <ctime>
#include <cstring>

bool CPvpUserRankingInfo::assign()
{
  m_vecPvpRankDataCurrent.assign(3, nullptr);
  m_vecPvpRankDataTomorrow.assign(3, nullptr);
  m_vecPackedRankData.assign(3, nullptr);
  m_dwUpdateRaceBossSerial.assign(3, nullptr);
  m_dwCurrentRaceBossSerial.assign(3, nullptr);

  for (int j = 0; j < 3; ++j)
  {
    _PVP_RANK_DATA *current = reinterpret_cast<_PVP_RANK_DATA *>(operator new[](0x157C));
    m_vecPvpRankDataCurrent[j] = current;
    if (current == nullptr)
    {
      return false;
    }
    std::memset(current, 0, 0x157C);

    _PVP_RANK_DATA *tomorrow = reinterpret_cast<_PVP_RANK_DATA *>(operator new[](0x157C));
    m_vecPvpRankDataTomorrow[j] = tomorrow;
    if (tomorrow == nullptr)
    {
      return false;
    }
    std::memset(tomorrow, 0, 0x157C);

    _PVP_RANK_PACKED_DATA *packed = reinterpret_cast<_PVP_RANK_PACKED_DATA *>(operator new[](0xF00));
    m_vecPackedRankData[j] = packed;
    if (packed == nullptr)
    {
      return false;
    }
    std::memset(packed, 0, 0xF00);

    unsigned long *updateSerial = reinterpret_cast<unsigned long *>(operator new[](0x24));
    m_dwUpdateRaceBossSerial[j] = updateSerial;
    if (updateSerial == nullptr)
    {
      return false;
    }
    std::memset(updateSerial, 0, 0x24);

    unsigned long *currentSerial = reinterpret_cast<unsigned long *>(operator new[](0x24));
    m_dwCurrentRaceBossSerial[j] = currentSerial;
    if (currentSerial == nullptr)
    {
      return false;
    }
    std::memset(currentSerial, 0, 0x24);
  }

  return true;
}

bool CPvpUserRankingTargetUserList::assign()
{
  m_PvpRankRefreshUser.assign(2532, nullptr);
  if (m_PvpRankRefreshUser.size() != 2532)
  {
    return false;
  }
  for (int j = 0; j < 2532; ++j)
  {
    _PVP_RANK_REFRESH_USER *entry = reinterpret_cast<_PVP_RANK_REFRESH_USER *>(operator new(0xC));
    m_PvpRankRefreshUser[j] = entry;
    if (m_PvpRankRefreshUser[j] == nullptr)
    {
      return false;
    }
  }
  return true;
}

void CUserRankingProcess::SetLogger(CLogFile *pkLogger)
{
  m_pkLogger = pkLogger;
  m_kGuildRanking.SetLogger(pkLogger);
}

void CUserRankingProcess::LoadINI(unsigned int *piHour, unsigned int *piMin)
{
  *piHour = GetPrivateProfileIntA("RankJob", "JobHour", 23, ".\\Initialize\\WorldSystem.ini");
  *piMin = GetPrivateProfileIntA("RankJob", "JobMin", 40, ".\\Initialize\\WorldSystem.ini");
}

bool CUserRankingProcess::SetRankingStartTime(int iHour, int iMin)
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }
  std::tm t = local;
  t.tm_hour = iHour;
  t.tm_min = iMin;
  t.tm_sec = 0;
  std::time_t start = std::mktime(&t);
  if (start == static_cast<std::time_t>(-1))
  {
    return false;
  }
  m_tStartTime = static_cast<long long>(start);
  return true;
}

bool CUserRankingProcess::AllocObject()
{
  return m_kTargetUserList.assign() && m_kPvpRankingInfo.assign();
}

bool CUserRankingProcess::InitProcFunc()
{
  void (CUserRankingProcess::*emptyProc)(void) = nullptr;
  m_vecProc.assign(10, emptyProc);
  if (m_vecProc.size() != 10)
  {
    return false;
  }
  m_vecProc[0] = &CUserRankingProcess::ProcWait;
  m_vecProc[1] = &CUserRankingProcess::ProcSaveTargetList;
  m_vecProc[2] = &CUserRankingProcess::ProcRankStart;
  m_vecProc[3] = &CUserRankingProcess::ProcRankComplete;
  m_vecProc[4] = &CUserRankingProcess::ProcWaitDayChanged;
  m_vecProc[5] = &CUserRankingProcess::ProcNotifyVersionUp;
  m_vecProc[6] = &CUserRankingProcess::ProcApplyGuildGrade;
  m_vecProc[7] = &CUserRankingProcess::ProcApplyRankInGuild;
  m_vecProc[8] = &CUserRankingProcess::ProcFailedWait;
  m_vecProc[9] = &CUserRankingProcess::ProcRankSuccess;
  return true;
}

bool CUserRankingProcess::Init()
{
  m_iOldDay = GetCurrentDay();
  if (m_iOldDay == -1)
  {
    if (m_pkLogger != nullptr)
    {
      m_pkLogger->Write("CUserRankingProcess::Init() : GetCurrentDay() Fail!");
    }
    return false;
  }

  unsigned int hour = 23;
  unsigned int min = 40;
  LoadINI(&hour, &min);
  if (!SetRankingStartTime(static_cast<int>(hour), static_cast<int>(min)))
  {
    if (m_pkLogger != nullptr)
    {
      m_pkLogger->Write(
        "CUserRankingProcess::Init() : SetRankingStartTime( iHour(%d), iMin(%d) ) Fail",
        hour,
        min);
    }
    return false;
  }

  if (!AllocObject())
  {
    if (m_pkLogger != nullptr)
    {
      m_pkLogger->Write("CUserRankingProcess::Init() : AllocObject() Fail!");
    }
    return false;
  }

  if (!InitProcFunc())
  {
    if (m_pkLogger != nullptr)
    {
      m_pkLogger->Write("CUserRankingProcess::Init() : InitProcFunc() Fail!");
    }
    return false;
  }

  return m_kGuildRanking.Init();
}

void CUserRankingProcess::ProcWait()
{
}

void CUserRankingProcess::ProcSaveTargetList()
{
}

void CUserRankingProcess::ProcRankStart()
{
}

void CUserRankingProcess::ProcRankComplete()
{
}

void CUserRankingProcess::ProcWaitDayChanged()
{
}

void CUserRankingProcess::ProcNotifyVersionUp()
{
}

void CUserRankingProcess::ProcApplyGuildGrade()
{
}

void CUserRankingProcess::ProcApplyRankInGuild()
{
}

void CUserRankingProcess::ProcFailedWait()
{
}

void CUserRankingProcess::ProcRankSuccess()
{
}
