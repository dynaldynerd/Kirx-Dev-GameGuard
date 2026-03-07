#include "pch.h"

#include "CGuildRoomSystem.h"

#include <cstring>

#include "CMapData.h"
#include "CMapOperation.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CRFWorldDatabase.h"
#include "guildroom_info.h"
#include "GlobalObjects.h"

CGuildRoomSystem::CGuildRoomSystem()
{
}

CGuildRoomSystem::~CGuildRoomSystem()
{
}

CGuildRoomSystem *CGuildRoomSystem::GetInstance()
{
  static CGuildRoomSystem s_instance;
  return &s_instance;
}

bool CGuildRoomSystem::Init()
{
  CGuildRoomInfo roomInfo;
  m_vecGuildRoom.assign(90u, roomInfo);
  if (m_vecGuildRoom.size() != 90)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : RACE_NUM*MAX_GUILD_ROOM != m_vecGuildRoom.size() Fail!");
    return false;
  }

  CMapData *map = nullptr;
  map = g_MapOper.GetMap("BellaGSD");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : bella_standard_map(%s) Not Exist Map Data!",
      "BellaGSD");
    return false;
  }
  m_pRoomMap[0][0] = map;

  map = g_MapOper.GetMap("CoraGSD");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : cora_standard_map(%s) Not Exist Map Data!",
      "CoraGSD");
    return false;
  }
  m_pRoomMap[1][0] = map;

  map = g_MapOper.GetMap("AccGSD");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : cora_standard_map(%s) Not Exist Map Data!",
      "CoraGSD");
    return false;
  }
  m_pRoomMap[2][0] = map;

  map = g_MapOper.GetMap("BellaGSP");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : bella_special_map(%s) Not Exist Map Data!",
      "BellaGSP");
    return false;
  }
  m_pRoomMap[0][1] = map;

  map = g_MapOper.GetMap("CoraGSP");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : cora_special_map(%s) Not Exist Map Data!",
      "CoraGSP");
    return false;
  }
  m_pRoomMap[1][1] = map;

  map = g_MapOper.GetMap("AccGSP");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : acc_special_map(%s) Not Exist Map Data!",
      "AccGSP");
    return false;
  }
  m_pRoomMap[2][1] = map;

  map = g_MapOper.GetMap("NeutralB");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : g_MapOper.GetMap(bella_neutral_map(%s)) is NULL!",
      "NeutralB");
    return false;
  }
  CGuildRoomInfo::sm_neutal_map[0] = map;

  _portal_dummy *portal = map->GetPortal(const_cast<char *>("dpfrom_bellato_camp"));
  if (!portal)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : pMap->GetPortal(bella_hq_dummy(%s)) is NULL!",
      "dpfrom_bellato_camp");
    return false;
  }
  if (!portal->m_pDumPos)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : Portal(%s)->m_pDumPos is NULL!",
      "dpfrom_bellato_camp");
    return false;
  }
  CGuildRoomInfo::sm_neutral_hq_dummy[0] = portal->m_pDumPos;

  map = g_MapOper.GetMap("NeutralC");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : g_MapOper.GetMap(cora_neutral_map(%s)) is NULL!",
      "NeutralC");
    return false;
  }
  CGuildRoomInfo::sm_neutal_map[1] = map;

  portal = map->GetPortal(const_cast<char *>("dpfrom_cora_camp"));
  if (!portal)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : pMap->GetPortal(cora_hq_dummy(%s)) is NULL!",
      "dpfrom_cora_camp");
    return false;
  }
  if (!portal->m_pDumPos)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : Portal(%s)->m_pDumPos is NULL!",
      "dpfrom_cora_camp");
    return false;
  }
  CGuildRoomInfo::sm_neutral_hq_dummy[1] = portal->m_pDumPos;

  map = g_MapOper.GetMap("NeutralA");
  if (!map)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : g_MapOper.GetMap(acc_neutral_map(%s)) is NULL!",
      "NeutralA");
    return false;
  }
  CGuildRoomInfo::sm_neutal_map[2] = map;

  portal = map->GetPortal(const_cast<char *>("dpfrom_accretia_camp"));
  if (!portal)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : pMap->GetPortal(acc_hq_dummy(%s)) is NULL!",
      "dpfrom_accretia_camp");
    return false;
  }
  if (!portal->m_pDumPos)
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Init() : Portal(%s)->m_pDumPos is NULL!",
      "dpfrom_accretia_camp");
    return false;
  }
  CGuildRoomInfo::sm_neutral_hq_dummy[2] = portal->m_pDumPos;

  for (int j = 0; j < 30; ++j)
  {
    if (j >= 20)
    {
      const int roomIndex = j - 20;
      m_vecGuildRoom[j].SetRoomMapInfo(m_pRoomMap[0][1], roomIndex, 1u, 0);
    }
    else
    {
      m_vecGuildRoom[j].SetRoomMapInfo(m_pRoomMap[0][0], j, 0, 0);
    }
  }

  for (int j = 0; j < 30; ++j)
  {
    if (j >= 20)
    {
      const int roomIndex = j - 20;
      m_vecGuildRoom[j + 30].SetRoomMapInfo(m_pRoomMap[1][1], roomIndex, 1u, 1u);
    }
    else
    {
      m_vecGuildRoom[j + 30].SetRoomMapInfo(m_pRoomMap[1][0], j, 0, 1u);
    }
  }

  for (int j = 0; j < 30; ++j)
  {
    if (j >= 20)
    {
      const int roomIndex = j - 20;
      m_vecGuildRoom[j + 60].SetRoomMapInfo(m_pRoomMap[2][1], roomIndex, 1u, 2u);
    }
    else
    {
      m_vecGuildRoom[j + 60].SetRoomMapInfo(m_pRoomMap[2][0], j, 0, 2u);
    }
  }

  CGuildRoomInfo::sm_RoomInfo[0].dwCost = 5000000;
  CGuildRoomInfo::sm_RoomInfo[0].dwTime = 604800;
  CGuildRoomInfo::sm_RoomInfo[0].byRestrict = 2;
  CGuildRoomInfo::sm_RoomInfo[0].byMaxRoomCount = 30;

  CGuildRoomInfo::sm_RoomInfo[1].dwCost = 20000000;
  CGuildRoomInfo::sm_RoomInfo[1].dwTime = 2592000;
  CGuildRoomInfo::sm_RoomInfo[1].byRestrict = 3;
  CGuildRoomInfo::sm_RoomInfo[1].byMaxRoomCount = 30;

  return true;
}

char CGuildRoomSystem::Load_db()
{
  _guildroom_info info{};
  memset_0(&info, 0, sizeof(info));

  if (!g_Main.m_pWorldDB->Select_GuildRoomInfo(&info))
  {
    g_Main.m_logLoadingError.Write(
      "CGuildRoomSystem::Load_db() : g_Main.m_pWorldDB->Select_GuildRoomInfo(...) Fail!");
    return 0;
  }

  for (unsigned __int8 j = 0; j < info.byCount; ++j)
  {
    if (RentRoom(
          info.info[j].byRace,
          info.info[j].byRoomType,
          0,
          info.info[j].dwGuildSerial,
          &info.info[j].ts,
          true))
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRoomSystem::Load_db() : RentRoom(...) %uth Fail!",
        j);
      return 0;
    }

    CMapData *map = GetMapData(info.info[j].byRace, info.info[j].byRoomType);
    if (!map)
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRoomSystem::Load_db() : CGuildRoomSystem::GetInstance()->GetMapData( byRace(%u), byRoomType(%u) ) %uth NULL!",
        info.info[j].byRace,
        info.info[j].byRoomType,
        j);
      return 0;
    }

    CMapItemStoreList *dest = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(map->m_nMapIndex);
    if (!dest)
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRoomSystem::Load_db() : CItemStoreManager::Instance()->GetMapItemStoreListBySerial(pMap->m_nMapIndex(%d)) %uth NULL!",
        map->m_nMapIndex,
        j);
      return 0;
    }

    CMapItemStoreList *base = CItemStoreManager::Instance()->GetEmptyInstanceItemStore();
    if (!base)
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRoomSystem::Load_db() : CItemStoreManager::Instance()->GetEmptyInstanceItemStore() %uth NULL!",
        j);
      return 0;
    }

    if (!base->CopyItemStoreData(dest))
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRoomSystem::Load_db() : pGuildStoreList->CopyItemStoreData(pBaseStoreList) %uth Fail!\r\n"
        "Map(%s) GuildRoom Store NPC Not Exist!",
        j,
        map->m_BspInfo.m_szBspName);
      return 0;
    }

    base->SetTypeNSerial(1u, info.info[j].dwGuildSerial);
  }

  return 1;
}

CMapData *CGuildRoomSystem::GetMapData(unsigned __int8 byRace, unsigned __int8 byMapType)
{
  if (byRace < 3u && byMapType < 2u)
  {
    return m_pRoomMap[byRace][byMapType];
  }

  return nullptr;
}

bool CGuildRoomSystem::GetMapPos(
  unsigned int dwGuildSerial,
  float *pPos,
  CMapData *pMap,
  unsigned __int16 *wMapLayer,
  unsigned __int8 *byRoomType)
{
for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      room.GetMapData();
      *wMapLayer = room.GetMapLayer();
      *byRoomType = room.GetRoomType();
      return room.GetMapPos(pPos);
    }
  }

  return false;
}

char CGuildRoomSystem::GetRestTime(unsigned int dwGuildSerial, int *tt)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      *tt = room.GetRestTime();
      return 1;
    }
  }
  return 0;
}

__int64 CGuildRoomSystem::GetRoomCountByType(unsigned __int8 byRace, unsigned __int8 byRoomType)
{
  unsigned int count = 0;
  int start = 30 * byRace;
  const int end = start + 30;
  while (start < end)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[start];
    if (room.IsRent() && room.GetRoomType() == byRoomType)
    {
      ++count;
    }
    ++start;
  }
  return count;
}

unsigned __int8 CGuildRoomSystem::GetRoomType(unsigned int dwGuildSerial)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      return room.GetRoomType();
    }
  }
  return 2;
}

char CGuildRoomSystem::IsGuildRoomMemberIn(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      if (room.IsMemberIn(n, dwCharSerial))
      {
        return 1;
      }
    }
  }
  return 0;
}

char CGuildRoomSystem::IsRoomRented(unsigned int dwGuildSerial)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      return 1;
    }
  }
  return 0;
}

unsigned __int8 CGuildRoomSystem::RentRoom(
  unsigned __int8 byRace,
  unsigned __int8 byRoomType,
  int iGuildInx,
  unsigned int dwGuildSerial,
  tagTIMESTAMP_STRUCT *ts,
  bool bRestore)
{
  if (byRace >= 3u || byRoomType >= 2u)
  {
    return 1;
  }

  const int roomCount = static_cast<int>(GetRoomCountByType(byRace, byRoomType));
  if (roomCount >= CGuildRoomInfo::sm_RoomInfo[byRoomType].byMaxRoomCount)
  {
    return 2;
  }

  int index = 30 * byRace;
  const int end = index + 30;
  while (index < end)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[index];
    if (!room.IsRent() && room.GetRoomType() == byRoomType)
    {
      if (bRestore)
      {
        tagTIMESTAMP_STRUCT restoreTs{};
        memcpy_0(&restoreTs, ts, sizeof(*ts));
        if (!room.SetRoom_Restore(iGuildInx, dwGuildSerial, &restoreTs))
        {
          return 3;
        }
      }
      else
      {
        if (!room.SetRoom(iGuildInx, dwGuildSerial))
        {
          return 3;
        }
      }
      break;
    }
    ++index;
  }

  if (index == end)
  {
    return 2;
  }

  return 0;
}

void CGuildRoomSystem::RentRoomTimer()
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent())
    {
      room.RoomTimer();
    }
  }
}

int CGuildRoomSystem::RoomIn(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      return room.RoomIn(n, dwCharSerial);
    }
  }
  return 3;
}

int CGuildRoomSystem::RoomOut(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial)
{
  for (int index = 0; index < 90; ++index)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[index];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      return room.RoomOut(n, dwCharSerial);
    }
  }
  return 3;
}

int CGuildRoomSystem::SetPlayerOut(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial)
{
  for (int j = 0; j < 90; ++j)
  {
    CGuildRoomInfo &room = m_vecGuildRoom[j];
    if (room.IsRent() && room.GetGuildSerial() == dwGuildSerial)
    {
      return room.SetPlayerOut(n, dwCharSerial, -1) ? 0 : 2;
    }
  }
  return 1;
}
