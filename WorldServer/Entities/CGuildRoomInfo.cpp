#include "pch.h"

#include "CGuildRoomInfo.h"

#include <ctime>

#include "CMapData.h"
#include "CMainThread.h"
#include "CItemStoreManager.h"
#include "CPlayer.h"
#include "CGuild.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"

CMapData *CGuildRoomInfo::sm_neutal_map[3];
_dummy_position *CGuildRoomInfo::sm_neutral_hq_dummy[3];
RoomInfo CGuildRoomInfo::sm_RoomInfo[2];

CGuildRoomInfo::CGuildRoomInfo()
{
  Room_Initialize();
}

CGuildRoomInfo::CGuildRoomInfo(const CGuildRoomInfo &other)
  : m_bRent(other.m_bRent),
    m_byRoomType(other.m_byRoomType),
    m_byRace(other.m_byRace),
    m_iGuildIdx(other.m_iGuildIdx),
    m_dwGuildSerial(other.m_dwGuildSerial),
    m_timeBegin(other.m_timeBegin),
    m_timer(other.m_timer),
    m_vecRoomMember(other.m_vecRoomMember),
    m_pRoomMap(other.m_pRoomMap),
    m_wRoomMapLayer(other.m_wRoomMapLayer),
    m_pLayerSet(other.m_pLayerSet),
    m_pRoomStartDummy(other.m_pRoomStartDummy)
{
}

CGuildRoomInfo &CGuildRoomInfo::operator=(const CGuildRoomInfo &other)
{
  if (this != &other)
  {
    m_bRent = other.m_bRent;
    m_byRoomType = other.m_byRoomType;
    m_byRace = other.m_byRace;
    m_iGuildIdx = other.m_iGuildIdx;
    m_dwGuildSerial = other.m_dwGuildSerial;
    m_timeBegin = other.m_timeBegin;
    m_timer = other.m_timer;
    m_vecRoomMember = other.m_vecRoomMember;
    m_pRoomMap = other.m_pRoomMap;
    m_wRoomMapLayer = other.m_wRoomMapLayer;
    m_pLayerSet = other.m_pLayerSet;
    m_pRoomStartDummy = other.m_pRoomStartDummy;
  }
  return *this;
}

CGuildRoomInfo::~CGuildRoomInfo() = default;

void CGuildRoomInfo::Room_Initialize()
{
  m_bRent = false;
  m_byRoomType = 2;
  m_iGuildIdx = -1;
  m_dwGuildSerial = 0;
  m_timeBegin = 0;
  m_timer = 0;
  m_vecRoomMember.clear();
}

void CGuildRoomInfo::SetRoomMapInfo(
  CMapData *pMap,
  unsigned __int16 wMapLayer,
  unsigned __int8 byRoomType,
  unsigned __int8 byRace)
{
  m_pRoomMap = pMap;
  m_wRoomMapLayer = wMapLayer;
  m_byRoomType = byRoomType;
  m_byRace = byRace;
  m_pRoomStartDummy = pMap->m_pPortal->m_pDumPos;
  m_pLayerSet = &pMap->m_ls[m_wRoomMapLayer];
}

bool CGuildRoomInfo::IsRent()
{
  return m_bRent;
}

unsigned int CGuildRoomInfo::GetGuildSerial()
{
  return m_dwGuildSerial;
}

CMapData *CGuildRoomInfo::GetMapData()
{
  return m_pRoomMap;
}

unsigned __int16 CGuildRoomInfo::GetMapLayer()
{
  return m_wRoomMapLayer;
}

unsigned __int8 CGuildRoomInfo::GetRoomType()
{
  return m_byRoomType;
}

bool CGuildRoomInfo::GetMapPos(float *pPos)
{
  return m_pRoomMap->GetRandPosInDummy(m_pRoomStartDummy, pPos, 1);
}

int CGuildRoomInfo::GetRestTime()
{
  __time32_t now{};
  _time32(&now);
  return static_cast<int>(m_timer - now);
}

bool CGuildRoomInfo::IsMemberIn(int n, unsigned int dwCharSerial)
{
  for (size_t j = 0; j < m_vecRoomMember.size(); ++j)
  {
    RoomCharInfo &member = m_vecRoomMember[j];
    if (member.bIn && member.iCharIdx == n && member.dwCharSerial == dwCharSerial)
    {
      return true;
    }
  }
  return false;
}

bool CGuildRoomInfo::SetRoom_Restore(int iGuildInx, unsigned int dwGuildSerial, tagTIMESTAMP_STRUCT *ts)
{
  if (IsRent())
  {
    return false;
  }

  RoomCharInfo value{};
  m_vecRoomMember.assign(100, value);
  if (m_vecRoomMember.size() != 100)
  {
    return false;
  }

  m_bRent = true;
  m_iGuildIdx = iGuildInx;
  m_dwGuildSerial = dwGuildSerial;

  _MULTI_BLOCK *pMB = m_pRoomMap->m_mb;
  m_pLayerSet->ActiveLayer(pMB);

  tagTIMESTAMP_STRUCT restoreTs{};
  std::memcpy(&restoreTs, ts, sizeof(restoreTs));
  SetRoomTime_Restore(&restoreTs);
  return true;
}

bool CGuildRoomInfo::SetRoom(int iGuildInx, unsigned int dwGuildSerial)
{
  if (IsRent())
  {
    return false;
  }

  RoomCharInfo value{};
  m_vecRoomMember.assign(100, value);
  if (m_vecRoomMember.size() != 100)
  {
    return false;
  }

  for (size_t j = 0; j < m_vecRoomMember.size(); ++j)
  {
    m_vecRoomMember[j].bIn = false;
  }

  m_bRent = true;
  m_iGuildIdx = iGuildInx;
  m_dwGuildSerial = dwGuildSerial;

  _MULTI_BLOCK *pMB = m_pRoomMap->m_mb;
  m_pLayerSet->ActiveLayer(pMB);
  SetRoomTime();
  SendDQS_RoomInsert();
  return true;
}

void CGuildRoomInfo::SetRoomTime()
{
  __time32_t now = 0;
  _time32(&now);
  m_timeBegin = static_cast<int>(now);
  m_timer = CGuildRoomInfo::sm_RoomInfo[m_byRoomType].dwTime + m_timeBegin;
}

void CGuildRoomInfo::SetRoomTime_Restore(tagTIMESTAMP_STRUCT *ts)
{
  std::tm tm{};
  tm.tm_year = ts->year - 1900;
  tm.tm_mon = ts->month - 1;
  tm.tm_mday = ts->day;
  tm.tm_hour = ts->hour;
  tm.tm_min = ts->minute;
  tm.tm_sec = ts->second;
  tm.tm_isdst = -1;

  m_timeBegin = _mktime32(&tm);
  if (m_timeBegin == -1)
  {
    m_timeBegin = 0;
  }
  m_timer = CGuildRoomInfo::sm_RoomInfo[m_byRoomType].dwTime + m_timeBegin;
}

void CGuildRoomInfo::SendDQS_RoomInsert()
{
  _qry_case_guildroom_insert query{};
  query.dwGuildSerial = m_dwGuildSerial;
  query.byRoomType = m_byRoomType;
  query.byRace = m_byRace;
  g_Main.PushDQSData(-1, nullptr, 71, reinterpret_cast<char *>(&query), sizeof(query));
}

void CGuildRoomInfo::SendDQS_RoomUpdate()
{
  _qry_case_guildroom_update query{};
  query.dwGuildSerial = m_dwGuildSerial;
  query.byRoomType = m_byRoomType;
  query.byRace = m_byRace;
  g_Main.PushDQSData(-1, nullptr, 72, reinterpret_cast<char *>(&query), sizeof(query));
}

void CGuildRoomInfo::SendMsg_RoomTimeOver()
{
  CGuild *guild = &g_Guild[m_iGuildIdx];
  if (guild && guild->m_dwSerial == m_dwGuildSerial)
  {
    guild->SendMsg_GuildRoomRented(100);
  }
}

void CGuildRoomInfo::OutAllRoomMember()
{
  for (int iMemberIdx = 0; iMemberIdx < static_cast<int>(m_vecRoomMember.size()); ++iMemberIdx)
  {
    if (m_vecRoomMember[iMemberIdx].bIn)
    {
      RoomCharInfo &member = m_vecRoomMember[iMemberIdx];
      SetPlayerOut(member.iCharIdx, member.dwCharSerial, iMemberIdx);
    }
  }
}

void CGuildRoomInfo::TimeOver()
{
  OutAllRoomMember();
  SendDQS_RoomUpdate();
  SendMsg_RoomTimeOver();
  CItemStoreManager::Instance()->ResetInstanceItemStore(1u, m_dwGuildSerial);
  Room_Initialize();
}

void CGuildRoomInfo::RoomTimer()
{
  __time32_t now{};
  _time32(&now);
  if (now >= m_timer)
  {
    TimeOver();
  }
}

int CGuildRoomInfo::RoomIn(int n, unsigned int dwCharSerial)
{
  if (IsMemberIn(n, dwCharSerial))
  {
    return 2;
  }

  for (size_t j = 0; j < m_vecRoomMember.size(); ++j)
  {
    if (!m_vecRoomMember[j].bIn)
    {
      m_vecRoomMember[j].bIn = true;
      m_vecRoomMember[j].iCharIdx = n;
      m_vecRoomMember[j].dwCharSerial = dwCharSerial;
      return 0;
    }
  }

  return 3;
}

int CGuildRoomInfo::RoomOut(int n, unsigned int dwCharSerial)
{
  for (size_t index = 0; index < m_vecRoomMember.size(); ++index)
  {
    RoomCharInfo &member = m_vecRoomMember[index];
    if (member.bIn && member.iCharIdx == n && member.dwCharSerial == dwCharSerial)
    {
      member.bIn = false;
      return 0;
    }
  }
  return 1;
}

bool CGuildRoomInfo::SetPlayerOut(int n, unsigned int dwCharSerial, int iMemberIdx)
{
  if (iMemberIdx >= 100)
  {
    return false;
  }

  if (iMemberIdx == -1)
  {
    for (size_t j = 0; j < m_vecRoomMember.size(); ++j)
    {
      RoomCharInfo &member = m_vecRoomMember[j];
      if (member.bIn && member.iCharIdx == n && member.dwCharSerial == dwCharSerial)
      {
        member.bIn = false;
        break;
      }
    }
  }
  else
  {
    m_vecRoomMember[iMemberIdx].bIn = false;
  }

  float newPos[8]{};
  if (!CGuildRoomInfo::sm_neutal_map[m_byRace]
      || !CGuildRoomInfo::sm_neutral_hq_dummy[m_byRace]
      || !CGuildRoomInfo::sm_neutal_map[m_byRace]->GetRandPosInDummy(
        CGuildRoomInfo::sm_neutral_hq_dummy[m_byRace],
        newPos,
        1))
  {
    return false;
  }

  CPlayer *player = &g_Player[n];
  if (player->m_bOper)
  {
    player->OutOfMap(CGuildRoomInfo::sm_neutal_map[m_byRace], 0, 3u, newPos);
    player->SendMsg_GuildRoomOutResult(
      10,
      CGuildRoomInfo::sm_neutal_map[m_byRace]->m_pMapSet->m_dwIndex,
      0,
      newPos);
    return true;
  }

  return false;
}
