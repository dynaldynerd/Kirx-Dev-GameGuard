#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CGuildRoomInfo.h"

class __cppobj CGuildRoomSystem
{
public:
  static CGuildRoomSystem *GetInstance();
  bool Init();
  int RoomOut(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial);

  std::vector<CGuildRoomInfo> m_vecGuildRoom;
  CMapData *m_pRoomMap[3][2];
};
