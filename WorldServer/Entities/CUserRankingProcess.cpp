#include "pch.h"

#include "CUserRankingProcess.h"

#include "GlobalObjects.h"
#include "qry_case_rank_racerank_guildrank.h"
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

bool CPvpUserRankingInfo::IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial)
{
  if (byRace >= m_dwCurrentRaceBossSerial.size())
  {
    return false;
  }

  for (int j = 0; j < 9; ++j)
  {
    unsigned long *serials = m_dwCurrentRaceBossSerial[byRace];
    if (serials && serials[j] == dwSerial)
    {
      return true;
    }
  }
  return false;
}

unsigned int CPvpUserRankingInfo::GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth)
{
  return m_dwCurrentRaceBossSerial[byRace][byNth];
}

unsigned __int8 CPvpUserRankingInfo::GetBossType(unsigned __int8 byRaceCode, unsigned int dwSerial)
{
  if (byRaceCode >= 3)
  {
    return static_cast<unsigned __int8>(-1);
  }

  for (int j = 0; j < 9; ++j)
  {
    unsigned long *serials = m_dwCurrentRaceBossSerial[byRaceCode];
    if (serials && serials[j] == dwSerial)
    {
      return static_cast<unsigned __int8>(j);
    }
  }
  return static_cast<unsigned __int8>(-1);
}

char CPvpUserRankingInfo::IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial)
{
  for (int j = 1; j < 9; ++j)
  {
    unsigned long *serials = m_dwCurrentRaceBossSerial[byRace];
    if (serials && serials[j] == dwSerial)
    {
      return 1;
    }
  }
  return 0;
}

void CPvpUserRankingInfo::ClearTomorrowPvpRankData()
{
  if (m_vecPvpRankDataTomorrow.empty())
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    _PVP_RANK_DATA *tomorrow = m_vecPvpRankDataTomorrow[j];
    if (tomorrow)
    {
      memset_0(tomorrow, 0, 0x157CuLL);
    }
  }
}

void CPvpUserRankingInfo::DoDayChangedWork(CLogFile *pkLogger)
{
  if (pkLogger)
  {
    pkLogger->Write("CPvpUserRankingInfo::DoDayChangedWork() Start");
  }

  if (m_vecPvpRankDataCurrent.empty() || m_vecPvpRankDataTomorrow.empty())
  {
    return;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (!m_vecPvpRankDataCurrent[j] || !m_vecPvpRankDataTomorrow[j])
    {
      return;
    }

    memcpy_0(m_vecPvpRankDataCurrent[j], m_vecPvpRankDataTomorrow[j], 0x157CuLL);
  }

  PvpRankDataPacking(pkLogger);
  ++m_byPvpRankDataVersion;

  if (pkLogger)
  {
    pkLogger->Write("CPvpUserRankingInfo::DoDayChangedWork() End");
  }
}

unsigned __int8 CPvpUserRankingInfo::GetPvpRankDataVersion()
{
  return m_byPvpRankDataVersion;
}

void CPvpUserRankingInfo::PvpRankDataPacking(CLogFile *pkLogger)
{
  if (pkLogger)
  {
    pkLogger->Write("CPvpUserRankingInfo::PvpRankDataPacking() Start");
  }

  for (int race = 0; race < 3; ++race)
  {
    unsigned int baseIndex = 0;
    for (int groupIndex = 0; groupIndex < 10; ++groupIndex)
    {
      _PVP_RANK_PACKED_DATA *packedData = m_vecPackedRankData[race];
      char *packedBuffer = packedData[groupIndex].szPackedData;
      unsigned __int16 dataLen = 0;

      for (unsigned int rankIndex = baseIndex; rankIndex < baseIndex + 10; ++rankIndex)
      {
        _PVP_RANK_DATA *rankData = m_vecPvpRankDataCurrent[race];
        unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(rankData[rankIndex].wszName) + 1);
        if (nameLen <= 1)
        {
          break;
        }

        packedBuffer[dataLen++] = static_cast<char>(nameLen);
        memcpy_0(packedBuffer + dataLen, rankData[rankIndex].wszName, nameLen);
        dataLen += nameLen;

        packedBuffer[dataLen++] = static_cast<char>(rankData[rankIndex].byRank);
        packedBuffer[dataLen++] = static_cast<char>(rankData[rankIndex].byGrade);

        unsigned __int8 guildNameLen =
          static_cast<unsigned __int8>(strlen_0(rankData[rankIndex].wszGuildName) + 1);
        packedBuffer[dataLen++] = static_cast<char>(guildNameLen);
        memcpy_0(packedBuffer + dataLen, rankData[rankIndex].wszGuildName, guildNameLen);
        dataLen += guildNameLen;

        packedBuffer[dataLen++] = static_cast<char>(
          GetBossType(static_cast<unsigned __int8>(race), rankData[rankIndex].dwAvatorSerial));
      }

      packedData[groupIndex].wDataLen = dataLen;
      baseIndex += 10;
    }
  }

  if (pkLogger)
  {
    pkLogger->Write("CPvpUserRankingInfo::PvpRankDataPacking() End");
  }
}

bool CPvpUserRankingTargetUserList::assign()
{
  m_PvpRankRefreshUser.assign(MAX_PLAYER, nullptr);
  if (m_PvpRankRefreshUser.size() != MAX_PLAYER)
  {
    return false;
  }
  for (int j = 0; j < MAX_PLAYER; ++j)
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

void CPvpUserRankingTargetUserList::ClearRankingStart()
{
  m_uiAddTotalCnt = 0;
  m_uiRefreshCnt = 0;

  for (_PVP_RANK_REFRESH_USER *entry : m_PvpRankRefreshUser)
  {
    entry->Init();
  }
}

void CPvpUserRankingTargetUserList::Add(unsigned int dwSerial, unsigned __int8 byLv, unsigned __int8 byRace)
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    _PVP_RANK_REFRESH_USER *entry = m_PvpRankRefreshUser[j];
    if (!entry->IsFilled())
    {
      for (int k = 0; k < j; ++k)
      {
        if (dwSerial == m_PvpRankRefreshUser[k]->dwSerial)
        {
          return;
        }
      }

      entry->SetData(dwSerial, byLv, byRace);
      break;
    }
  }

  ++m_uiAddTotalCnt;
}

unsigned int CPvpUserRankingTargetUserList::GetAddedTotalCnt()
{
  return m_uiAddTotalCnt;
}

void CPvpUserRankingTargetUserList::UpdateCharGrade()
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bOper)
    {
      for (int k = 0; k < MAX_PLAYER; ++k)
      {
        _PVP_RANK_REFRESH_USER *entry = m_PvpRankRefreshUser[k];
        if (entry->IsFilled() && entry->wPvpRate != 0xFFFF)
        {
          const unsigned int charSerial = player->m_Param.GetCharSerial();
          if (entry->dwSerial == charSerial)
          {
            unsigned __int8 grade = CPlayerDB::CalcCharGrade(entry->byLv, entry->wPvpRate);
            player->SetGrade(grade);
            player->SetRankRate(entry->wPvpRate, entry->dwPvpRank);
            break;
          }
        }
      }
    }
  }
}

void _PVP_RANK_REFRESH_USER::Init()
{
  dwSerial = static_cast<unsigned int>(-1);
}

bool _PVP_RANK_REFRESH_USER::IsFilled()
{
  return dwSerial != static_cast<unsigned int>(-1);
}

void _PVP_RANK_REFRESH_USER::SetData(unsigned int dwSerialP, unsigned __int8 byLvP, unsigned __int8 byRaceP)
{
  dwSerial = dwSerialP;
  byLv = byLvP;
  byRace = byRaceP;
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

void CUserRankingProcess::UpdateNextRankingStartTime()
{
  m_tStartTime += 86400LL;
}

bool CUserRankingProcess::IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kPvpRankingInfo.IsCurrentRaceBossGroup(byRace, dwSerial);
}

unsigned int CUserRankingProcess::GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth)
{
  return m_kPvpRankingInfo.GetCurrentRaceBossSerial(byRace, byNth);
}

unsigned __int8 CUserRankingProcess::GetBossType(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kPvpRankingInfo.GetBossType(byRace, dwSerial);
}

bool CUserRankingProcess::IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kPvpRankingInfo.IsRaceViceBoss(byRace, dwSerial) != 0;
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
  std::time_t now = std::time(nullptr);
  if (static_cast<long long>(now) >= m_tStartTime)
  {
    m_iPvpSaveJobIndexOffset = 0;
    m_iPvpRankDataVersionUpSendOffset = 0;
    m_kTargetUserList.ClearRankingStart();
    m_kPvpRankingInfo.ClearTomorrowPvpRankData();
    m_pkLogger->WriteString("CUserRankingProcess::ProcWait() End");
    m_eState = PS_SAVE_TARGET_LIST;
  }
}

void CUserRankingProcess::ProcSaveTargetList()
{
  m_pkLogger->Write("CUserRankingProcess::ProcSaveTargetList() Start");
  int index = m_iPvpSaveJobIndexOffset;
  for (; index < MAX_PLAYER; ++index)
  {
    CUserDB *user = &g_UserDB[index];
    if (user->m_dwSerial != static_cast<unsigned int>(-1)
        && user->m_AvatorData.dbAvator.m_zClassHistory[0] != -1
        && !user->m_byUserDgr)
    {
      m_kTargetUserList.Add(
        user->m_dwSerial,
        user->m_AvatorData.dbAvator.m_byLevel,
        user->m_AvatorData.dbAvator.m_byRaceSexCode);
      m_iPvpSaveJobIndexOffset = index + 1;
      break;
    }
  }

  if (index >= MAX_PLAYER)
  {
    unsigned int total = m_kTargetUserList.GetAddedTotalCnt();
    m_pkLogger->Write("CUserRankingProcess::ProcSaveTargetList() End : %d", total);
    m_eState = PS_RANK_START;
  }
}

void CUserRankingProcess::ProcRankStart()
{
  m_pkLogger->Write("CUserRankingProcess::ProcRankStart() Start");
  for (int j = 0; j < MAX_GUILD; ++j)
  {
    CGuild *guild = &g_Guild[j];
    if (guild->IsFill())
    {
      guild->ClearVote();
      guild->StartRankJob();
    }
  }

  char szDate[48];
  memset_0(szDate, 0, 9u);
  GetDateStrAfterDay(szDate, 9, 1u);

  _qry_case_rank_racerank_guildrank qry;
  strcpy_s(qry.szDate, 9uLL, szDate);
  g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x55u, qry.szDate, 20);
  m_eState = PS_USER_RANK_PROCESS;
  m_pkLogger->WriteString("CUserRankingProcess::ProcRankStart() End\r\n");
}

void CUserRankingProcess::ProcRankComplete()
{
  m_eState = PS_WAIT_DAY_CHANGED;
  m_pkLogger->WriteString("CUserRankingProcess::ProcRankComplete() Start, End");
}

void CUserRankingProcess::ProcWaitDayChanged()
{
  if (IsDayChanged(&m_iOldDay))
  {
    m_pkLogger->WriteString("CUserRankingProcess::ProcRankStart() Start");
    m_kPvpRankingInfo.DoDayChangedWork(m_pkLogger);
    m_kTargetUserList.UpdateCharGrade();
    m_iPvpRankDataVersionUpSendOffset = 0;
    m_eState = PS_NOTIFY_VERSIONUP;
    m_pkLogger->WriteString("CUserRankingProcess::ProcRankStart() End");
  }
}

void CUserRankingProcess::ProcNotifyVersionUp()
{
  m_pkLogger->WriteString("CUserRankingProcess::ProcNotifyVersionUp() Start");
  int sentCount = 0;
  unsigned __int8 version = m_kPvpRankingInfo.GetPvpRankDataVersion();

  int index = m_iPvpRankDataVersionUpSendOffset;
  for (; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bLive)
    {
      player->SendMsg_PvpRankListVersionUp(static_cast<char>(version));
      if (++sentCount >= 10)
      {
        m_iPvpRankDataVersionUpSendOffset = index + 1;
        break;
      }
    }
  }

  if (index >= MAX_PLAYER)
  {
    _qry_case_rank_racerank_guildrank qry;
    GetDateStrAfterDay(qry.szDate, 9, 1u);
    m_kGuildRanking.ClearGuildSerial();
    m_kGuildRanking.ClearGuildGrade();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x6Bu, qry.szDate, 20);
    m_eState = PS_UPDATE_AND_LOAD_GRADE;
    m_iPvpRankDataVersionUpSendOffset = 0;
    m_pkLogger->WriteString("CUserRankingProcess::ProcNotifyVersionUp() End");
  }
}

void CUserRankingProcess::ProcApplyGuildGrade()
{
  m_pkLogger->WriteString("CUserRankingProcess::ProcNotifyVersionUp() Start");
  m_kGuildRanking.ApplyGuildGrade();
  m_kGuildRanking.ClearApplyRankInGuildJobOffset();
  m_eState = PS_APPLY_RANK_IN_GUILD;
  m_pkLogger->WriteString("CUserRankingProcess::ProcNotifyVersionUp() End");
}

void CUserRankingProcess::ProcApplyRankInGuild()
{
  static unsigned int s_rankInGuildTh = 0;
  const unsigned int rankInGuildOffset = m_kGuildRanking.GetRankInGuildJobOffset();
  m_pkLogger->Write(
    "CUserRankingProcess::ProcApplyRankInGuild() %uTh, Offset(%d)",
    s_rankInGuildTh,
    rankInGuildOffset);
  ++s_rankInGuildTh;

  if (m_kGuildRanking.ApplyRankInGuild())
  {
    m_eState = PS_RANK_SUCCESS;
    m_pkLogger->WriteString("CUserRankingProcess::ProcApplyRankInGuild() End");
  }
}

void CUserRankingProcess::ProcFailedWait()
{
  m_pkLogger->WriteString("CUserRankingProcess::ProcFailedWait() Start");
  for (int j = 0; j < MAX_GUILD; ++j)
  {
    CGuild *guild = &g_Guild[j];
    if (guild->IsFill())
    {
      guild->EndRankJob();
    }
  }
  UpdateNextRankingStartTime();
  m_eState = PS_WAIT;
  m_pkLogger->WriteString("CUserRankingProcess::ProcFailedWait() End");
}

void CUserRankingProcess::ProcRankSuccess()
{
  m_pkLogger->WriteString("CUserRankingProcess::ProcRankSuccess() Start");
  for (int j = 0; j < MAX_GUILD; ++j)
  {
    CGuild *guild = &g_Guild[j];
    if (guild->IsFill())
    {
      guild->EndRankJob();
    }
  }
  UpdateNextRankingStartTime();
  m_eState = PS_WAIT;
  m_pkLogger->WriteString("CUserRankingProcess::ProcRankSuccess() End");
}
