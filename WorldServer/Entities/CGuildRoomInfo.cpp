#include "pch.h"

#include "CGuildRoomInfo.h"

#include "CMapData.h"

CMapData *CGuildRoomInfo::sm_neutal_map[3] = {};
_dummy_position *CGuildRoomInfo::sm_neutral_hq_dummy[3] = {};
RoomInfo CGuildRoomInfo::sm_RoomInfo[2] = {};

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
