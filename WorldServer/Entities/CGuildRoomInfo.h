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

class __cppobj CGuildRoomInfo
{
public:
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
};
