#pragma once

#include "IdaCompat.h"

class CGuild;
class CPlayer;

class __cppobj CGuildBattleController
{
public:
  static CGuildBattleController *Instance();
  bool Init();
  unsigned __int8 Add(
    CGuild *pSrcGuild,
    CGuild *pDestGuild,
    unsigned int dwStartTime,
    unsigned __int8 byNumber,
    unsigned int dwMapInx);
  unsigned __int8 Add(
    CGuild *pSrcGuild,
    CGuild *pDestGuild,
    unsigned int dwStartTime,
    unsigned int dwElapseTimeCnt,
    unsigned __int8 byNumber,
    unsigned int dwMapInx);
  void AddComplete(unsigned __int8 byResult, unsigned int uiMapID, unsigned int dwID, unsigned int dwSLID);
  unsigned __int8 AddSchedule(char *szData);
  bool UpdateWinLose(
    unsigned __int8 byWinRace,
    unsigned int dwWinGuildSerial,
    unsigned __int8 byLoseRace,
    unsigned int dwLoseGuildSerial);
  bool UpdateDraw(
    unsigned __int8 by1PRace,
    unsigned int dw1PGuildSerial,
    unsigned __int8 by2PRace,
    unsigned int dw2PGuildSerial);
  bool UpdateRank(unsigned __int8 byRace, unsigned __int8 *byOutData);
  void CompleteUpdateRank(unsigned __int8 byResult, unsigned __int8 byRace, unsigned __int8 *pLoadData);
  bool UpdateClearRerservedDayInfo(
    unsigned int dwStartSLID,
    unsigned int dwEndSLID,
    unsigned int dwStartSID,
    unsigned int dwEndSID);
  bool UpdateReservedGuildBattleSchedule(unsigned int dwSLID, unsigned __int8 *byOutData);
  void CompleteUpdateReservedSchedule(unsigned int dwMapID, unsigned __int8 *pLoadData);
  void CompleteCreateGuildBattleRankTable(unsigned __int8 byResult);
  void CompleteClearGuildBattleRank(unsigned __int8 byResult);
  void PushCreateGuildBattleRankTable();
  void PushClearGuildBattleRank();
  void LogIn(CPlayer *pkPlayer);
  void NetClose(CPlayer *pkPlayer);
  void SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace);

  virtual ~CGuildBattleController() = default;
};

