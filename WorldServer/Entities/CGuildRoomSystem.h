#pragma once

#include "IdaCompat.h"

#include <vector>

#include "CGuildRoomInfo.h"

class CMapData;
struct tagTIMESTAMP_STRUCT;

class  CGuildRoomSystem
{
public:
  CGuildRoomSystem();
  ~CGuildRoomSystem();

  static CGuildRoomSystem *GetInstance();
  bool Init();
  char Load_db();
  CMapData *GetMapData(unsigned __int8 byRace, unsigned __int8 byMapType);
  bool GetMapPos(
    unsigned int dwGuildSerial,
    float *pPos,
    CMapData *pMap,
    unsigned __int16 *wMapLayer,
    unsigned __int8 *byRoomType);
  char GetRestTime(unsigned int dwGuildSerial, int *tt);
  __int64 GetRoomCountByType(unsigned __int8 byRace, unsigned __int8 byRoomType);
  unsigned __int8 GetRoomType(unsigned int dwGuildSerial);
  char IsGuildRoomMemberIn(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial);
  char IsRoomRented(unsigned int dwGuildSerial);
  unsigned __int8 RentRoom(
    unsigned __int8 byRace,
    unsigned __int8 byRoomType,
    int iGuildInx,
    unsigned int dwGuildSerial,
    tagTIMESTAMP_STRUCT *ts,
    bool bRestore);
  void RentRoomTimer();
  int RoomIn(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial);
  int RoomOut(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial);
  int SetPlayerOut(unsigned int dwGuildSerial, int n, unsigned int dwCharSerial);

  std::vector<CGuildRoomInfo> m_vecGuildRoom;
  CMapData *m_pRoomMap[3][2];
};
