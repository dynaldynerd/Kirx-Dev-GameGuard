#include "pch.h"

#include "CUserRankingProcess.h"

#include "GlobalObjects.h"
#include "pvp_rank_list_result_data_zocl.h"
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

void CPvpUserRankingInfo::SetUpdateRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial)
{
  if (byNth < 9)
  {
    m_dwUpdateRaceBossSerial[byRace][byNth] = dwSerial;
  }
}

void CPvpUserRankingInfo::FlipPvPRankTop()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int nth = 0; nth < 9; ++nth)
    {
      unsigned long *updateSerials = m_dwUpdateRaceBossSerial[race];
      if (updateSerials && updateSerials[nth] != static_cast<unsigned int>(-1))
      {
        if (!IsCurrentRaceBossGroup(static_cast<unsigned __int8>(race), updateSerials[nth]))
        {
          CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, updateSerials[nth]);
          if (player)
          {
            player->SendMsg_RaceTopInform(1);
          }
        }
      }
    }
  }

  for (int race = 0; race < 3; ++race)
  {
    for (int nth = 0; nth < 9; ++nth)
    {
      unsigned long *currentSerials = m_dwCurrentRaceBossSerial[race];
      if (currentSerials && currentSerials[nth] != static_cast<unsigned int>(-1))
      {
        if (!IsCurrentRaceBossGroup(static_cast<unsigned __int8>(race), currentSerials[nth]))
        {
          CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, currentSerials[nth]);
          if (player)
          {
            player->SendMsg_RaceTopInform(0);
          }
        }
      }
    }
  }

  for (size_t race = 0; race < m_dwUpdateRaceBossSerial.size(); ++race)
  {
    for (int nth = 0; nth < 9; ++nth)
    {
      unsigned long *updateSerials = m_dwUpdateRaceBossSerial[race];
      unsigned long *currentSerials = m_dwCurrentRaceBossSerial[race];
      currentSerials[nth] = updateSerials[nth];
    }
  }
}

void CPvpUserRankingInfo::IncreaseVesion()
{
  ++m_byPvpRankDataVersion;
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

void CPvpUserRankingInfo::PvpRankListRequest(
  unsigned __int16 wIndex,
  unsigned __int8 byRace,
  unsigned __int8 byVersion,
  unsigned __int8 byPage)
{
  if (m_byPvpRankDataVersion == byVersion)
  {
    SendMsg_PvpRankListNodata(wIndex, static_cast<char>(byRace), static_cast<char>(byPage), 0);
  }
  else
  {
    SendMsg_PvpRankListData(wIndex, byRace, m_byPvpRankDataVersion, byPage);
  }
}

void CPvpUserRankingInfo::SendMsg_PvpRankListData(
  unsigned __int16 wIndex,
  unsigned __int8 byRace,
  unsigned __int8 byVersion,
  unsigned __int8 byPage)
{
  if (byRace >= 3u || byPage >= 10u)
  {
    return;
  }

  _PVP_RANK_PACKED_DATA *packed = &(m_vecPackedRankData[byRace][byPage]);
  if (!packed)
  {
    return;
  }

  if (!packed->wDataLen)
  {
    SendMsg_PvpRankListNodata(wIndex, static_cast<char>(byRace), static_cast<char>(byPage), 1u);
  }

  _pvp_rank_list_result_data_zocl result;
  result.byRace = byRace;
  result.byVersion = byVersion;
  result.byPage = byPage;
  result.wRankDataLen = packed->wDataLen;
  memcpy_0(result.szPvpRankData, packed->szPackedData, packed->wDataLen);

  unsigned __int8 type[2]{13, 20};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, reinterpret_cast<char *>(&result), len);
}

void CPvpUserRankingInfo::SendMsg_PvpRankListNodata(
  unsigned __int16 wIndex,
  char byRace,
  char byPage,
  unsigned __int8 byRet)
{
  char payload[3]{byRace, byPage, static_cast<char>(byRet)};
  unsigned __int8 type[2]{13, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, payload, 3u);
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

void CUserRankingProcess::Loop()
{
  if (m_eState > PS_RANK_SUCCESS)
  {
    if (m_eState < PS_USER_RANK_PROCESS || m_eState > PS_UPDATE_AND_LOAD_GRADE || m_eState == PS_NONE)
    {
      if (m_pkLogger)
      {
        m_pkLogger->Write("CUserRankingProcess::Loop() : m_eState(%d) Invalid State!", m_eState);
      }
      m_eState = PS_INVALID;
    }
    return;
  }

  if (m_eState < 0 || static_cast<size_t>(m_eState) >= m_vecProc.size())
  {
    if (m_pkLogger)
    {
      m_pkLogger->Write("CUserRankingProcess::Loop() : m_eState(%d) Invalid Proc Index!", m_eState);
    }
    m_eState = PS_INVALID;
    return;
  }

  const auto proc = m_vecProc[static_cast<size_t>(m_eState)];
  if (proc)
  {
    (this->*proc)();
  }
}

void CUserRankingProcess::SetUpdateRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth, unsigned int dwSerial)
{
  m_kPvpRankingInfo.SetUpdateRaceBossSerial(byRace, byNth, dwSerial);
}

void CUserRankingProcess::FlipPvPRankTop()
{
  m_kPvpRankingInfo.FlipPvPRankTop();
}

void CUserRankingProcess::PvpRankDataPacking()
{
  m_kPvpRankingInfo.PvpRankDataPacking(m_pkLogger);
}

void CUserRankingProcess::PvpRankListRequest(
  unsigned __int16 wIndex,
  unsigned __int8 byRace,
  unsigned __int8 byVersion,
  unsigned __int8 byPage)
{
  m_kPvpRankingInfo.PvpRankListRequest(wIndex, byRace, byVersion, byPage);
}

void CUserRankingProcess::IncreaseVesion()
{
  m_kPvpRankingInfo.IncreaseVesion();
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

char CUserRankingProcess::Load()
{
  m_kPvpRankingInfo.IncreaseVesion();
  m_eState = PS_WAIT;
  return 1;
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

_PVP_RANK_DATA *CPvpUserRankingInfo::GetTomorrowPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth)
{
  if (m_vecPvpRankDataTomorrow.size() > byRace && byNth < 0x64u)
  {
    return &m_vecPvpRankDataTomorrow[byRace][byNth];
  }

  return nullptr;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep1(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step1(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep2(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step2(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep3(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step3(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep4(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step4(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep5(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step5(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep6(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step6(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep7(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step7(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep8(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step8(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep9(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RaceRank_Step9(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CPvpUserRankingInfo::UpdateRaceRankStep10(char *szData)
{
  for (int j = 0; j < 3; ++j)
  {
    _PVP_RANK_DATA *rankData = GetTomorrowPvpRankData(static_cast<unsigned __int8>(j), 0);
    if (!rankData || g_Main.m_pWorldDB->Select_PvpRankInfo(j, szData, rankData))
    {
      szData[9] = 1;
      *reinterpret_cast<unsigned int *>(szData + 12) = static_cast<unsigned int>(j);
      return 0;
    }
  }

  return 0;
}

unsigned __int8 CPvpUserRankingTargetUserList::UpdateRaceRankStep11(char *szData)
{
  unsigned __int8 result = 0;

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    _PVP_RANK_REFRESH_USER *entry = m_PvpRankRefreshUser[j];
    if (entry->IsFilled())
    {
      unsigned __int16 *rankRate = &entry->wPvpRate;
      unsigned int *rank = &entry->dwPvpRank;
      result = g_Main.m_pWorldDB->Select_PvpRate(entry->dwSerial, szData, rank, rankRate);
      if (result == 2)
      {
        entry->wPvpRate = static_cast<unsigned __int16>(-1);
      }
      else if (result == 1)
      {
        *reinterpret_cast<unsigned int *>(szData + 12) = static_cast<unsigned int>(j);
        *reinterpret_cast<unsigned int *>(szData + 16) = entry->dwSerial;
        return 0;
      }

      ++m_uiRefreshCnt;
    }
  }

  return 0;
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep1(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep1(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep2(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep2(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep3(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep3(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep4(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep4(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep5(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep5(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep6(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep6(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep7(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep7(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep8(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep8(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep9(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep9(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep10(char *szData)
{
  return m_kPvpRankingInfo.UpdateRaceRankStep10(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRaceRankStep11(char *szData)
{
  return m_kTargetUserList.UpdateRaceRankStep11(szData);
}

unsigned __int8 CUserRankingProcess::UpdateGuildRankStep1(char *szData)
{
  return m_kGuildRanking.UpdateGuildRankStep1(szData);
}

unsigned __int8 CUserRankingProcess::UpdateGuildRankStep2(char *szData)
{
  return m_kGuildRanking.UpdateGuildRankStep2(szData);
}

unsigned __int8 CUserRankingProcess::UpdateGuildRankStep3(char *szData)
{
  return m_kGuildRanking.UpdateGuildRankStep3(szData);
}

unsigned __int8 CUserRankingProcess::UpdateGuildRankStep4(char *szData)
{
  return m_kGuildRanking.UpdateGuildRankStep4(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep1(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep1(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep2(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep2(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep3(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep3(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep4(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep4(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep5(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep5(szData);
}

unsigned __int8 CUserRankingProcess::UpdateRankinGuildStep6(char *szData)
{
  return m_kGuildRanking.UpdateRankinGuildStep6(szData);
}

unsigned __int8 CUserRankingProcess::UpdateAndSelectGuildGrade(char *szData)
{
  return m_kGuildRanking.UpdateAndSelectGuildGrade(szData);
}

void CUserRankingProcess::CompleteRaceRankStep1(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep1(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step1 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x56u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep1(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step1 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep2(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep1(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step2 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x57u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep2(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step2 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep3(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep3(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step3 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x58u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep3(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step3 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep4(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep4(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step4 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x59u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep4(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step4 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep5(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep5(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step5 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Au, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep5(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step5 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep6(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep6(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step6 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Bu, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep6(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step6 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep7(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep7(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step7 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Cu, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep7(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step7 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep8(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep8(...) : %s Date qry_case_rank_racerank_step8 Fail!",
      qryData->szDate);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Du, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep8(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step8 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep9(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep9(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step9 Fail!\r\n",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Eu, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep9(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step9 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteRaceRankStep10(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep10(...) : %s Date byRet(%u) byProcRet(%u)  qry_case_rank_racerank_step10 Fail!\r\n"
      "\t\tLoad Race Top100 Rank : %d Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet,
      qryData->dwParam1);
  }

  qryData->ClearRetParam();
  g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x5Fu, qryData->szDate, 20);
  m_pkLogger->Write(
    "CUserRankingProcess::CompleteRaceRankStep10(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step10 Success!",
    qryData->szDate,
    byRet,
    qryData->scProcRet);
}

void CUserRankingProcess::CompleteRaceRankStep11(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRaceRankStep11(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step11 Fail!\r\n"
      "\t\t(%u)th %u Select_PvpRate Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet,
      qryData->dwParam1,
      qryData->dwParam2);
  }

  qryData->ClearRetParam();
  g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x61u, qryData->szDate, 20);
  m_eState = PS_GUILD_RANK_PROCESS;
  m_pkLogger->Write(
    "CUserRankingProcess::CompleteRaceRankStep11(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step11 Success!\r\n",
    qryData->szDate,
    byRet,
    qryData->scProcRet);
}

void CUserRankingProcess::CompleteGuildRankStep1(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep1(...) : \r\n%s Date byRet(%u) byProcRet(%u) qry_case_rank_guildrank_step1 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x62u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep1(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step1 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteGuildRankStep2(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep2(...) : \r\n%s Date byRet(%u) byProcRet(%u) qry_case_rank_guildrank_step2 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x63u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep2(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step2 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteGuildRankStep3(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep3(...) : \r\n%s Date byRet(%u) byProcRet(%u) qry_case_rank_guildrank_step3 Fail!",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    m_kGuildRanking.ClearGuildSerial();
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x64u, qryData->szDate, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep3(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step3 Success!",
      qryData->szDate,
      0,
      qryData->scProcRet);
  }
}

void CUserRankingProcess::CompleteGuildRankStep4(unsigned __int8 byRet, char *szData)
{
  auto *qryData = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (byRet || qryData->scProcRet)
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteGuildRankStep4(...) : \r\n"
      "\t\t%s Date byRet(%u) byProcRet(%u) qry_case_rank_guildrank_step4 Fail!\r\n"
      "\t\tSelectAllGuildSerial Fail",
      qryData->szDate,
      byRet,
      qryData->scProcRet);
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    qryData->ClearRetParam();
    qryData->dwParam1 = m_kGuildRanking.GetNextGuildSerial();
    if (qryData->dwParam1 == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->Write(
        "CUserRankingProcess::CompleteGuildRankStep4(...) : m_eState = PS_RANK_COMPLETE");
    }
    else
    {
      m_kGuildRanking.ClearRefreshData();
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, qryData->szDate, 20);
      m_eState = PS_RANK_IN_GUILD_PROCESS;
      m_pkLogger->Write(
        "CUserRankingProcess::CompleteGuildRankStep4(...) : %s Date byRet(%u) byProcRet(%u) qry_case_rank_racerank_step4 Success!\r\n",
        qryData->szDate,
        0,
        qryData->scProcRet);
    }
  }
}

void CUserRankingProcess::CompleteRankInGuildStep1(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep1(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step1 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));

    unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
    *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
    if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep1(...) : m_eState = PS_RANK_COMPLETE\r\n");
    }
    else
    {
      pQryData[9] = 0;
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep1(...) : Push qry_case_rank_rankinguild_step1\r\n");
    }
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x66u, pQryData, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep1(...) : Push qry_case_rank_rankinguild_step2");
  }
}

void CUserRankingProcess::CompleteRankInGuildStep2(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep2(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step2 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));

    unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
    *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
    if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep2(...) : m_eState = PS_RANK_COMPLETE\r\n");
    }
    else
    {
      pQryData[9] = 0;
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep2(...) : Push qry_case_rank_rankinguild_step1\r\n");
    }
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x67u, pQryData, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep2(...) : Push qry_case_rank_rankinguild_step3");
  }
}

void CUserRankingProcess::CompleteRankInGuildStep3(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep3(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step3 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));

    unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
    *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
    if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep3(...) : m_eState = PS_RANK_COMPLETE\r\n");
    }
    else
    {
      pQryData[9] = 0;
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep3(...) : Push qry_case_rank_rankinguild_step1\r\n");
    }
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x68u, pQryData, 20);
    m_pkLogger->WriteString(
      "CUserRankingProcess::CompleteRankInGuildStep3(...) : Push qry_case_rank_rankinguild_step4");
  }
}

void CUserRankingProcess::CompleteRankInGuildStep4(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep4(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step4 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));

    unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
    *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
    if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep4(...) : m_eState = PS_RANK_COMPLETE\r\n");
    }
    else
    {
      pQryData[9] = 0;
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep4(...) : Push qry_case_rank_rankinguild_step1\r\n");
    }
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x69u, pQryData, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep4(...) : Push qry_case_rank_rankinguild_step5");
  }
}

void CUserRankingProcess::CompleteRankInGuildStep5(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep5(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step5 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));

    unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
    *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
    if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
    {
      m_eState = PS_RANK_COMPLETE;
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep5(...) : m_eState = PS_RANK_COMPLETE\r\n");
    }
    else
    {
      pQryData[9] = 0;
      g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
      m_pkLogger->WriteString(
        "CUserRankingProcess::CompleteRankInGuildStep5(...) : Push qry_case_rank_rankinguild_step1\r\n");
    }
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x6Au, pQryData, 20);
    m_pkLogger->WriteString(
      "CUserRankingProcess::CompleteRankInGuildStep5(...) : Push qry_case_rank_rankinguild_step6");
  }
}

void CUserRankingProcess::CompleteRankInGuildStep6(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep6(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_rankinguild_step6 Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));
  }
  else
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep6(...) : SUCCESS : Update_RankInGuild Serial(%u)",
      *reinterpret_cast<unsigned int *>(pQryData + 12));
  }

  unsigned int nextSerial = m_kGuildRanking.GetNextGuildSerial();
  *reinterpret_cast<unsigned int *>(pQryData + 12) = nextSerial;
  if (*reinterpret_cast<unsigned int *>(pQryData + 12) == static_cast<unsigned int>(-1))
  {
    m_eState = PS_RANK_COMPLETE;
    m_pkLogger->WriteString(
      "CUserRankingProcess::CompleteRankInGuildStep6(...) : m_eState = PS_RANK_COMPLETE\r\n");
  }
  else
  {
    pQryData[9] = 0;
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x65u, pQryData, 20);
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankInGuildStep6(...) : Push qry_case_rank_guildrank_step1 Serial(%u)\r\n",
      *reinterpret_cast<unsigned int *>(pQryData + 12));
  }
}

void CUserRankingProcess::CompleteRankUpdateAndSelectGarde(unsigned __int8 byRet, char *szData)
{
  char *pQryData = szData;
  if (byRet || pQryData[9])
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankUpdateAndSelectGarde(...) : %s Date byRet(%u) byProcRet(%u) Guild(%u) qry_case_rank_update_and_select_garde Fail!",
      pQryData,
      byRet,
      pQryData[9],
      *reinterpret_cast<unsigned int *>(pQryData + 12));
    m_eState = PS_FAILED_WAIT;
  }
  else
  {
    m_pkLogger->Write(
      "CUserRankingProcess::CompleteRankUpdateAndSelectGarde(...) : SUCCESS : Update_GuildGrade");
    m_eState = PS_APPLY_GUILD_GRADE;
  }
}
