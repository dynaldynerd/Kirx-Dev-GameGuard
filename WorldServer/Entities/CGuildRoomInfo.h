#pragma once

#include "IdaCompat.h"

#include <vector>

class CMapData;
struct _LAYER_SET;
struct _dummy_position;

struct RoomCharInfo
{
  bool bIn;
  int iCharIdx;
  unsigned int dwCharSerial;
};

struct __declspec(align(4)) RoomInfo
{
  unsigned int dwCost;
  unsigned int dwTime;
  unsigned __int8 byRestrict;
  unsigned __int8 byMaxRoomCount;
};

class __cppobj CGuildRoomInfo
{
public:
  CGuildRoomInfo();
  CGuildRoomInfo(const CGuildRoomInfo &other);
  CGuildRoomInfo &operator=(const CGuildRoomInfo &other);
  ~CGuildRoomInfo();

  void Room_Initialize();
  void SetRoomMapInfo(CMapData *pMap, unsigned __int16 wMapLayer, unsigned __int8 byRoomType, unsigned __int8 byRace);

  bool m_bRent;
  unsigned __int8 m_byRoomType;
  unsigned __int8 m_byRace;
  int m_iGuildIdx;
  unsigned int m_dwGuildSerial;
  int m_timeBegin;
  int m_timer;
  std::vector<RoomCharInfo> m_vecRoomMember;
  CMapData *m_pRoomMap;
  unsigned __int16 m_wRoomMapLayer;
  _LAYER_SET *m_pLayerSet;
  _dummy_position *m_pRoomStartDummy;

  static CMapData *sm_neutal_map[3];
  static _dummy_position *sm_neutral_hq_dummy[3];
  static RoomInfo sm_RoomInfo[2];
};
