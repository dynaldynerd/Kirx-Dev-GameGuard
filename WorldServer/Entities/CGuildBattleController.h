#pragma once

#include "IdaCompat.h"

class CGuild;
class CPlayer;

class __cppobj CGuildBattleController
{
public:
  CGuildBattleController();

  static CGuildBattleController *Instance();
  bool Init();
  void Loop();
  void Clear();
  void Flip();
  char Load();
  char LoadINI(
    unsigned int *uiMapCnt,
    unsigned int *iToday,
    unsigned int *iTodayDayID,
    int *iTomorrow,
    int *iTomorrowDayID);
  char SaveINI();
  unsigned __int8 IsAvailableSuggest(
    CGuild *pSrcGuild,
    unsigned int dwDestGuild,
    unsigned int dwStartTime,
    unsigned int dwNumber,
    unsigned int dwMapCode);
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
  void UpdatePossibleBattleGuildList();
  void JoinGuild(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
  void LeaveGuild(CPlayer *pkPlayer);
  unsigned __int8 Start(CPlayer *pkPlayer);
  void LogIn(CPlayer *pkPlayer);
  void NetClose(CPlayer *pkPlayer);
  void SendPossibleBattleGuildList(int n, unsigned __int8 byRace, unsigned __int8 byPage, unsigned int dwVer);
  void SendRankList(
    int n,
    unsigned __int8 bySelfRace,
    unsigned int dwCurVer,
    unsigned __int8 uiMapID,
    unsigned __int8 byPage,
    unsigned int dwGuildSerial);
  void Join(CPlayer *pkPlayer);
  void SendReservedScheduleList(
    int n,
    unsigned int uiMapID,
    unsigned int dwVer,
    unsigned __int8 byDay,
    unsigned __int8 byPage,
    unsigned int dwGuildSerial);
  void SendCurrentBattleInfoRequest(int n, unsigned int uiMapID);
  void CheckTakeGravityStone(int iPortalInx, CPlayer *pkPlayer);
  void CheckGetGravityStone(unsigned __int16 wIndex, unsigned int dwObjSerial, CPlayer *pkPlayer);
  void CheckGoal(CPlayer *pkPlayer, int iPortalInx);
  int CheatRegenStone(CPlayer *pkPlayer, int iRengenPos);
  char CheatTakeStone(int iPortalInx, CPlayer *pkPlayer);
  char CheatGetStone(CPlayer *pkPlayer);
  void SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace);

  virtual ~CGuildBattleController() = default;
};

