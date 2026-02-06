#include "pch.h"

#include "CGuildRoomSystem.h"

#include "CMapData.h"
#include "CMapOperation.h"
#include "GlobalObjects.h"

CGuildRoomSystem *CGuildRoomSystem::GetInstance()
{
  static CGuildRoomSystem s_instance;
  return &s_instance;
}

bool CGuildRoomSystem::Init()
{
  CGuildRoomInfo roomInfo;
  m_vecGuildRoom.assign(0x5Au, roomInfo);
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
