#pragma once

#include "IdaCompat.h"

#include <atltime.h>
#include <vector>

class CPlayer;
class CGuild;
class CMapData;
class CLogFile;
class CMyTimer;
class CCircleZone;
class CGravityStoneRegener;
class CGravityStone;
struct _dummy_position;
struct _guild_member_info;
struct _possible_battle_guild_list_result_zocl;
struct _guild_battle_rank_list_result_zocl;
struct _guild_battle_reserved_schedule_result_zocl;
struct _guild_battle_current_battle_info_result_zocl;
struct _worlddb_guild_battle_rank_list;
struct _worlddb_guild_battle_schedule_list;
struct _worlddb_guild_battle_info;
struct _worlddb_guild_battle_reserved_schedule_info;
struct _base_fld;

namespace GUILD_BATTLE
{
  class CGuildBattle;
  class CGuildBattleState;
  class CGuildBattleStateList;
  class CGuildBattleSchedule;
  class CGuildBattleSchedulePool;
  class CGuildBattleReservedSchedule;
  class CGuildBattleReservedScheduleMapGroup;
  class CGuildBattleScheduleManager;
  class CGuildBattleLogger;
  class CGuildBattleRewardItem;
  class CGuildBattleRewardItemManager;
  class CGuildBattleRankManager;
  class CNormalGuildBattle;
  class CNormalGuildBattleLogger;
  class CNormalGuildBattleStateList;
  class CNormalGuildBattleStateListPool;
  class CNormalGuildBattleField;
  class CNormalGuildBattleFieldList;
  class CNormalGuildBattleGuild;
  class CNormalGuildBattleGuildMember;
  class CNormalGuildBattleManager;
  class CPossibleBattleGuildListManager;
  class CGuildBattleScheduler;
  class CGuildBattleReservedScheduleListManager;
  class CReservedGuildScheduleDayGroup;
  class CReservedGuildScheduleMapGroup;
  class CReservedGuildSchedulePage;
  class CCurrentGuildBattleInfoManager;

  extern unsigned __int8 LIMIT_SRC_GRADE;
  extern unsigned __int8 LIMIT_DEST_GRADE;

  class CGuildBattle
  {
  public:
    virtual ~CGuildBattle() = default;
    virtual int GetObjType();
  };

  class CGuildBattleState
  {
  public:
    virtual ~CGuildBattleState() = default;
    virtual int Enter(CGuildBattle *pkBattle);
    virtual int Loop(CGuildBattle *pkBattle);
    virtual int Fin(CGuildBattle *pkBattle);
    virtual ATL::CTimeSpan *GetTerm(ATL::CTimeSpan *result);
  };

  class CGuildBattleStateList
  {
  public:
    enum GBS_LOOP_TYPE : __int32
    {
      GBS_ONCE = 0,
      GBS_LOOP = 1,
      GBS_COUNT = 2,
      GBS_MAX = 3,
    };
    enum STATE : __int32
    {
      STATE_NONE = -3,
      STATE_WAIT = -2,
      STATE_READY = -1,
    };

    CGuildBattleStateList(int iStateMax, GBS_LOOP_TYPE iLoopType, unsigned int uiLoopCnt);
    virtual ~CGuildBattleStateList() = default;
    virtual void SetNextState();
    void Clear();
    void SetWait();
    void SetReady();

    int m_iForceAdvance;
    int m_iState;
    CGuildBattleState *m_pkNextState;
    CGuildBattleState *m_pkCurState;
    const int STATE_MAX;
    unsigned int m_uiLoopCnt;
    unsigned int m_uiCurLoopCnt;
    GBS_LOOP_TYPE m_eLoopType;
    bool m_bEnter;
  };

  class CNormalGuildBattleState : public CGuildBattleState
  {
  public:
    CNormalGuildBattleState();
  };

  class CNormalGuildBattleStateNotify : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateNotify();
  };

  class CNormalGuildBattleStateReady : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateReady();
  };

  class CNormalGuildBattleStateCountDown : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateCountDown();
  };

  class CNormalGuildBattleStateRound : public CGuildBattleState
  {
  public:
    CNormalGuildBattleStateRound();
  };

  class CNormalGuildBattleStateRoundStart : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundStart();

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundProcess : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundProcess();

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundReturnStartPos : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundReturnStartPos();

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundList : public CGuildBattleStateList
  {
  public:
    CNormalGuildBattleStateRoundList();
    bool IsInBattleRegenState();

    CNormalGuildBattleStateRoundStart START;
    CNormalGuildBattleStateRoundProcess PROCESS;
    CNormalGuildBattleStateRoundReturnStartPos ROUND_END;
    CNormalGuildBattleStateRound *m_pStateList[3];
  };

  class CNormalGuildBattleStateInBattle : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateInBattle();
    void SetBattleTime(ATL::CTimeSpan kTime);
    bool IsInBattleRegenState();

    ATL::CTimeSpan m_kInBattleTime;
    CNormalGuildBattleStateRoundList m_kRountStateList;
  };

  class CNormalGuildBattleStateDivide : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateDivide();
  };

  class CNormalGuildBattleStateReturn : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateReturn();
  };

  class CNormalGuildBattleStateFin : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateFin();
  };

  class CNormalGuildBattleStateList : public CGuildBattleStateList
  {
  public:
    CNormalGuildBattleStateList();
    bool IsReadyOrCountState();
    bool IsInBattle();
    bool IsInBattleRegenState();
    void SetBattleTime(ATL::CTimeSpan kTime);

    CNormalGuildBattleStateNotify NOTIFY;
    CNormalGuildBattleStateReady READY;
    CNormalGuildBattleStateCountDown COUNT;
    CNormalGuildBattleStateInBattle INBATTLE;
    CNormalGuildBattleStateDivide DIVIDE;
    CNormalGuildBattleStateReturn RETURN;
    CNormalGuildBattleStateFin FIN;
    CNormalGuildBattleState *m_pStateList[7];
  };

  class CNormalGuildBattleStateListPool
  {
  public:
    static CNormalGuildBattleStateListPool *Instance();
    CNormalGuildBattleStateListPool();
    bool Init();
    CNormalGuildBattleStateList *Get(unsigned int dwID);

    static CNormalGuildBattleStateListPool *ms_Instance;
    unsigned int m_dwMaxCount;
    CNormalGuildBattleStateList *m_pkStateList;
  };

  class CNormalGuildBattleField
  {
  public:
    bool Init(unsigned int uiMapInx);
    bool Start(unsigned __int8 byStartPos, CPlayer *pkPlayer);
    char MoveStartPos(unsigned __int8 byStartPos, unsigned __int8 byMapOutType, CPlayer *pkPlayer);
    unsigned __int8 DropBall(CPlayer *pkPlayer);
    unsigned int GetMapCode();
    unsigned int GetMapID();

    bool m_bInit;
    unsigned int m_uiMapInx;
    CMapData *m_pkMap;
    _dummy_position *m_pkStartPos[2];
    unsigned int m_ui1PGoalPosCnt;
    CCircleZone *m_pk1PGoalZone;
    unsigned int m_ui2PGoalPosCnt;
    CCircleZone *m_pk2PGoalZone;
    unsigned int m_uiRegenPosCnt;
    CGravityStoneRegener *m_pkRegenPos;
    CGravityStone *m_pkBall;
  };

  class CNormalGuildBattleFieldList
  {
  public:
    static CNormalGuildBattleFieldList *Instance();
    CNormalGuildBattleFieldList();
    bool Init();
    bool InitUseField(unsigned __int8 byRace, char *szKeyName, char *szStrBuff, char **szParseBuff);
    unsigned int GetMapCnt();
    char GetMapInx(unsigned __int8 byRace, unsigned int dwMapCode, unsigned int *dwMapInx);
    __int64 GetMapInxList(unsigned __int8 byRace, unsigned __int8 *pbyInxList);
    CNormalGuildBattleField *GetField(unsigned int dwMapID);
    CNormalGuildBattleField *GetField(unsigned __int8 byRace, unsigned int dwMapCode);

    static CNormalGuildBattleFieldList *ms_Instance;
    unsigned int m_dwCnt;
    CNormalGuildBattleField *m_pkField;
    unsigned __int8 m_byUseFieldCnt[3];
    CNormalGuildBattleField **m_ppkUseFieldByRace[3];
  };

  class CNormalGuildBattleLogger
  {
  public:
    CNormalGuildBattleLogger();
    bool Init();
    void CreateLogFile(char *szLogName);
    void Log(char *fmt, ...);

    CLogFile *m_pkLogger;
  };

  class CNormalGuildBattleGuildMember
  {
  public:
    CNormalGuildBattleGuildMember();
    void Clear();
    bool IsEmpty();
    bool IsExist();
    bool IsEnableStart();
    unsigned int GetSerial();
    unsigned __int16 GetIndex();
    CPlayer *GetPlayer();
    void StockOldInfo();
    void SetBattleState(bool bFlag, unsigned __int8 byColorInx);
    void Send(unsigned __int8 *byType, char *pSend, unsigned __int16 uiSize);
    void CleanUpBattle();
    void NetClose();
    void ReturnBindPos();

    unsigned int m_dwSerial;
    bool m_bRestart;
    CMapData *m_pOldBindMapData;
    _dummy_position *m_pOldBindDummyData;
    char m_szOldBindMapCode[12];
    char m_szOldBindDummy[12];
    unsigned __int16 m_usGoalCnt;
    unsigned __int16 m_usKillCnt;
    long double m_dPvpPoint;
    _guild_member_info *m_pkMember;
  };

  class CNormalGuildBattleGuild
  {
  public:
    explicit CNormalGuildBattleGuild(unsigned __int8 byID);
    void Clear();
    void SetGuild(CGuild *pkGuild);
    CGuild *GetGuild();
    unsigned int GetGuildSerial();
    char *GetGuildName();
    unsigned __int8 GetGuildRace();
    unsigned __int8 GetColorInx();
    char IsJoinMember(unsigned int dwSerial);
    __int64 GetMember(unsigned int dwSerial);
    void AskJoin(int n, unsigned int dwSerial, unsigned __int8 GuildBattleNumber, char *wszDestGuild, CNormalGuildBattleLogger *kLogger);
    void AskJoin(unsigned int n, char *wszDestGuildName, CNormalGuildBattleLogger *kLogger);
    void LogIn(
      int n,
      unsigned int dwSerial,
      unsigned __int8 GuildBattleNumber,
      char *wszDestGuild,
      unsigned int uiID,
      CNormalGuildBattleField *pkField,
      CNormalGuildBattleLogger *kLogger);
    void SendMsg(unsigned __int8 *byType, char *pMsg, unsigned __int16 uiSize);
    void SendMsg(unsigned __int8 *byType, char *pMsg, unsigned __int16 uiSize, int iExeceptMemberInx);
    void SendMsg(unsigned __int8 *byType, char *pMsg, unsigned __int16 uiSize, unsigned int dwSerial);
    void SendOhterNotifyCommitteeMemberPosition(CPlayer *pkPlayer);
    void SendSelfNotifyCommitteeMemberPositionList(CPlayer *pkPlayer);
    void SendDeleteNotifyPositionMember(int iMemberInx);
    char MoveMember(int iMember, unsigned int uiID, CNormalGuildBattleField *pkField, CNormalGuildBattleLogger *kLogger);
    char NetClose(bool bInGuildBattle, unsigned int dwSerial, CNormalGuildBattleLogger *kLogger);
    bool IsMember(unsigned int dwSerial);
    void LeaveGuild(unsigned int dwSerial, bool bInGuildBattle, CNormalGuildBattleLogger *kLogger);
    void ReturnBindPosAll();
    char *GetANSIGuildName();

    const unsigned __int8 m_byID;
    unsigned __int8 m_byColorInx;
    unsigned int m_dwGoalCnt;
    unsigned int m_dwScore;
    unsigned int m_dwKillPoint;
    CGuild *m_pkGuild;
    unsigned __int8 m_byNotifyPositionMemberCnt;
    CNormalGuildBattleGuildMember *m_pkNotifyPositionMember[10];
    unsigned int m_dwKillCountSum;
    unsigned int m_dwMaxJoinMemberCnt;
    unsigned int m_dwCurJoinMember;
    CNormalGuildBattleGuildMember m_kMember[150];
  };

  class CNormalGuildBattle : public CGuildBattle
  {
  public:
    CNormalGuildBattle(unsigned int dwID);
    bool CreateLogger();
    void CreateLogFile();
    void Clear();
    char Init(
      bool bToday,
      unsigned int uiDayID,
      unsigned int dwID,
      unsigned int dwP1GuildSerial,
      unsigned int dwP2GuildSerial,
      unsigned int dwMapID,
      unsigned __int8 byNumber);
    void Init(CGuild *pk1P, CGuild *pk2P, CNormalGuildBattleField *pkField, unsigned __int8 byNumber, CNormalGuildBattleStateList *pkStateList);
    void AddComplete(unsigned __int8 byRet);
    void SendWebAddScheduleInfo();
    bool IsEmpty();
    bool IsMemberGuild(unsigned int dwGuildSerial);
    bool IsReadyOrCountState();
    bool IsInBattle();
    void AskJoin(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    void LogIn(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 NetClose(unsigned int dwCharacSerial, CPlayer *pkPlayer);
    unsigned __int8 LeaveGuild(CPlayer *pkPlayer);
    void NotifyDestoryBall(unsigned int dwOwnerSerial);
    unsigned int GetID();
    CNormalGuildBattleGuild *Get1P();
    CNormalGuildBattleGuild *Get2P();
    unsigned __int8 GetGuildBattleNumber();

    const unsigned int m_dwID;
    bool m_bInit;
    CNormalGuildBattleLogger m_kLogger;
    unsigned __int8 m_byGuildBattleNumber;
    CNormalGuildBattleGuild m_k1P;
    CNormalGuildBattleGuild m_k2P;
    CNormalGuildBattleField *m_pkField;
    unsigned __int8 m_byWinResult;
    CNormalGuildBattleGuild *m_pkWin;
    CNormalGuildBattleGuild *m_pkLose;
    CNormalGuildBattleGuild *m_pkRed;
    CNormalGuildBattleGuild *m_pkBlue;
    CNormalGuildBattleStateList *m_pkStateList;
  };

  class CNormalGuildBattleManager
  {
  public:
    static CNormalGuildBattleManager *Instance();
    CNormalGuildBattleManager();
    bool Init();
    bool Load(int iCurDay, unsigned int uiOldMapCnt, int iToday, unsigned int iTodayDayID, int iTomorrow, unsigned int iTomorrowDayID);
    char Load(bool bToday, unsigned int uiDayID, CNormalGuildBattle **ppkStart);
    char LoadDBGuildBattleInfo(unsigned int dwStartID, _worlddb_guild_battle_info *kInfo);
    char AddDefaultDBRecord();
    void Clear();
    void Clear(CNormalGuildBattle **ppkStart);
    void JoinGuild(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 LeaveGuild(CPlayer *pkPlayer);
    unsigned __int8 Add(
      CGuild *pSrcGuild,
      CGuild *pDestGuild,
      unsigned int dwStartTime,
      unsigned int dwElapseTimeCnt,
      unsigned __int8 byNumber,
      unsigned int dwMapCode);
    void AddComplete(unsigned __int8 byRet, unsigned int dwBattleID);
    void LogIn(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 NetClose(unsigned int dwGuildSerial, unsigned int dwCharacSerial, CPlayer *pkPlayer);
    void CheckTakeGravityStone(int iPortalInx, unsigned int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    void CheckGetGravityStone(
      unsigned __int16 wIndex,
      unsigned int dwObjSerial,
      unsigned int n,
      unsigned int dwGuildSerial,
      unsigned int dwCharacSerial);
    void CheckGoal(unsigned int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial, int iPortalInx);
    unsigned __int8 ProcCheckTakeGravityStone(int iPortalInx, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 ProcCheckGetGravityStone(
      unsigned __int16 wIndex,
      unsigned int dwObjSerial,
      unsigned int dwGuildSerial,
      unsigned int dwCharacSerial);
    unsigned __int8 ProcCheckGoal(unsigned int dwGuildSerial, unsigned int dwCharacSerial, int iPortalInx);
    CNormalGuildBattle *GetBattleByGuildSerial(unsigned int dwGuildSerial);
    CNormalGuildBattle *GetBattle(unsigned int dwID);
    bool PushDQSData(
      unsigned int uiFieldInx,
      unsigned int uiSLID,
      CNormalGuildBattle *pkBattle,
      CGuildBattleSchedule *pkSchedule);
    char UpdateClearGuildBattleDayInfo(unsigned int dwStartSID, unsigned int dwEndSID);

    static CNormalGuildBattleManager *ms_Instance;
    bool m_bLoad;
    bool m_bDone;
    unsigned int m_uiMapCnt;
    unsigned int m_uiMaxBattleCnt;
    CNormalGuildBattle **m_ppkNormalBattle;
    CNormalGuildBattle **m_ppkTodayBattle;
    CNormalGuildBattle **m_ppkTomorrowBattle;
  };

  class CGuildBattleSchedule
  {
  public:
    CGuildBattleSchedule(unsigned int dwScheduleID);
    enum GS_STATE : __int32
    {
      GS_NONE = 0,
      GS_WAIT = 1,
      GS_PROC = 2,
      GS_DONE = 3,
      GS_MAX = 4,
    };

    unsigned int GetSID();
    ATL::CTimeSpan *GetBattleTime(ATL::CTimeSpan *result);
    void SetStateList(CGuildBattleStateList *pkStateList);
    unsigned int GetState();
    __int64 GetRealStartTime();
    __int64 GetBattleTurm();
    bool IsProc();
    char GetLeftTime(unsigned __int8 *byHour, unsigned __int8 *byMin, unsigned __int8 *bySec);
    char Load(bool bToday, unsigned int dwScheduleID, unsigned __int8 ucState, __int64 tTime, unsigned __int16 wTumeMin);
    ATL::CTime *GetTime(ATL::CTime *result);
    unsigned __int8 Set(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt);
    bool IsEmpty();
    bool IsDone();
    void Clear();

    unsigned int m_dwScheduleID;
    GS_STATE m_eState;
    ATL::CTime m_kNextStartTime;
    ATL::CTime m_kBattleStartTime;
    ATL::CTimeSpan m_kBattleTime;
    CGuildBattleStateList *m_pkStateList;
  };

  class CGuildBattleSchedulePool
  {
  public:
    static CGuildBattleSchedulePool *Instance();
    CGuildBattleSchedulePool();
    bool Init(unsigned int uiMapCnt);
    CGuildBattleSchedule *Get(unsigned int dwSID);
    CGuildBattleSchedule *Get(unsigned int uiSLID, unsigned int dwStartInx);
    CGuildBattleSchedule *GetRef(unsigned int dwSID);
    void ClearByDayID(unsigned int uiDayID);

    static CGuildBattleSchedulePool *ms_Instance;
    unsigned int m_uiMapCnt;
    unsigned int m_dwMaxScheduleCnt;
    CGuildBattleSchedule **m_ppkSchedule;
  };

  class CGuildBattleReservedSchedule
  {
  public:
    CGuildBattleReservedSchedule(unsigned int uiScheduleListID);
    void Clear();
    unsigned int GetID();
    unsigned __int8 Add(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt, CGuildBattleSchedule **ppkSchedule);
    unsigned __int8 IsEmptyTime(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt);
    void UpdateUseField(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt);
    char Load(bool bToday, _worlddb_guild_battle_schedule_list *pkInfo);
    CGuildBattleSchedule *UpdateUseFlag(unsigned int dwID);
    void Flip();
    void ClearElapsedSchedule();
    char CleanUpDanglingReservedSchedule();
    unsigned int GetCurScheduleID();

    unsigned int m_uiScheduleListID;
    bool m_bDone;
    unsigned int m_uiCurScheduleInx;
    bool m_bUseField[23];
    CGuildBattleSchedule *m_pkSchedule[23];
  };

  class CGuildBattleReservedScheduleMapGroup
  {
  public:
    CGuildBattleReservedScheduleMapGroup();
    bool Init(unsigned int uiDayInx, unsigned int uiMapCnt);
    char Load(bool bToday);
    unsigned __int8 Add(
      unsigned int uiFieldInx,
      unsigned int dwStartTimeInx,
      unsigned int dwElapseTimeCnt,
      CGuildBattleSchedule **ppkSchedule,
      unsigned int *uiSLID);
    unsigned __int8 IsEmptyTime(unsigned int uiFieldInx, unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt);
    bool Clear();
    char Clear(unsigned int uiMapID, unsigned int dwID);
    CGuildBattleSchedule *UpdateUseFlag(unsigned int uiMapID, unsigned int dwID);
    unsigned int GetDayID();
    char GetSLID(unsigned int uiMapID, unsigned int *uiSLID);
    char CleanUpDanglingReservedSchedule();
    unsigned int GetCurScheduleID(unsigned int uiMapID);
    void Flip();

    static _worlddb_guild_battle_schedule_list *ms_pkDBScheduleInfo;
    bool m_bDone;
    unsigned int m_uiDayInx;
    unsigned int m_uiMapCnt;
    CGuildBattleReservedSchedule **m_ppkReservedSchedule;
  };

  class CGuildBattleScheduleManager
  {
  public:
    static CGuildBattleScheduleManager *Instance();
    CGuildBattleScheduleManager();
    bool Init(unsigned int uiMapCnt);
    char Load(int iCurDay, unsigned int uiOldMapCnt, int iToday, int iTodayDayID, int iTomorrow, int iTomorrowDayID);
    unsigned __int8 Add(
      unsigned int uiFieldInx,
      unsigned int dwStartTimeInx,
      unsigned int dwElapseTimeCnt,
      CGuildBattleSchedule **ppkSchedule,
      unsigned int *uiSLID);
    bool ClearTommorowScheduleByID(unsigned int uiMapID, unsigned int dwID);
    unsigned int GetTodayDayID();
    unsigned int GetTomorrowDayID();
    unsigned __int8 IsEmptyTime(unsigned int uiFieldInx, unsigned int dwStartTimeInx);
    char AddDefaultDBTable();
    CGuildBattleSchedule *UpdateUseFlag(unsigned int uiDayID, unsigned int uiMapID, unsigned int dwID);
    bool GetTodaySLIDByMap(unsigned int uiMap, unsigned int *uiSLID);
    bool GetTomorrowSLIDByMap(unsigned int uiMap, unsigned int *uiSLID);
    bool CleanUpDanglingReservedSchedule();
    unsigned int GetCurScheduleID(unsigned int uiMapID);

    bool m_bLoad;
    ATL::CTime *m_pkOldDayTime;
    CMyTimer *m_pkTimer;
    unsigned int m_uiMapCnt;
    CGuildBattleReservedScheduleMapGroup m_kSchdule[2];
    CGuildBattleReservedScheduleMapGroup *m_pkTodaySchedule;
    CGuildBattleReservedScheduleMapGroup *m_pkTomorrowSchedule;
  };

  class CGuildBattleLogger
  {
  public:
    static CGuildBattleLogger *Instance();
    CGuildBattleLogger();
    bool Init();
    void CreateLogFile(char *szLogName);
    void Log(const char *fmt, ...);

    static CGuildBattleLogger *ms_Instance;
    CLogFile *m_pkLogger;
  };

  class CGuildBattleRewardItem
  {
  public:
    CGuildBattleRewardItem();
    bool Init(unsigned __int16 usInx);
    bool SetItem(char *szItemCode);
    const CGuildBattleRewardItem *Give(CPlayer *pkPlayer);
    char *GetItemCode();
    unsigned __int8 GetAmount();
    bool IsNull();

    static CGuildBattleRewardItem ms_kNullObj;
    unsigned __int8 m_ucD;
    unsigned __int8 m_ucTableCode;
    _base_fld *m_pFld;
  };

  class CGuildBattleRewardItemManager
  {
  public:
    static CGuildBattleRewardItemManager *Instance();
    CGuildBattleRewardItemManager();
    bool Init();
    const CGuildBattleRewardItem *Give(CPlayer *pkPlayer);

    static CGuildBattleRewardItemManager *ms_Instance;
    std::vector<CGuildBattleRewardItem> m_kItem;
  };

  class CGuildBattleRankManager
  {
  public:
    static CGuildBattleRankManager *Instance();
    CGuildBattleRankManager();
    bool Init();
    char Load();
    char Load(unsigned __int8 byRace);
    bool CheckRecord(unsigned int dwGuildSerial);
    char UpdateWinLose(
      unsigned __int8 byWinRace,
      unsigned int dwWinGuildSerial,
      unsigned __int8 byLoseRace,
      unsigned int dwLoseGuildSerial);
    char UpdateDraw(
      unsigned __int8 by1PRace,
      unsigned int dw1PGuildSerial,
      unsigned __int8 by2PRace,
      unsigned int dw2PGuildSerial);
    bool SelectGuildBattleRankList(unsigned __int8 byRace, _worlddb_guild_battle_rank_list *pOutData);
    char Update(unsigned __int8 byRace, unsigned __int8 *pLoadData);
    void Send(
      unsigned int n,
      unsigned __int8 bySelfRace,
      unsigned int dwCurVer,
      char byTabRace,
      unsigned __int8 byPage,
      unsigned int dwGuildSerial);
    char Find(unsigned __int8 byRace, unsigned int dwGuildSerial, int *iFindInx, unsigned __int8 *byFindPage);
    void PushCreateGuildBattleRankTable();
    void PushClearGuildBattleRank();
    void Clear();
    void Clear(unsigned __int8 byRace);

    static CGuildBattleRankManager *ms_Instance;
    unsigned int m_dwVer[3];
    unsigned int m_dwGuildSerial[3][30][10];
    _guild_battle_rank_list_result_zocl **m_ppkList;
  };

  class CReservedGuildSchedulePage
  {
  public:
    CReservedGuildSchedulePage();
    bool Init(unsigned __int8 ucPageInx);
    bool Clear();
    char Find(unsigned int dwGuildSerial);
    void Send(unsigned int n, unsigned int dwVer, CReservedGuildSchedulePage *pkSelfInfoPage);
    void IncVer();
    void Flip();

    unsigned int m_dw1PGuildSerial[5];
    unsigned int m_dw2PGuildSerial[5];
    _guild_battle_reserved_schedule_result_zocl *m_pkList;
    unsigned __int8 m_ucPageInx;
    unsigned int m_dwVer;
  };

  class CReservedGuildScheduleMapGroup
  {
  public:
    CReservedGuildScheduleMapGroup();
    bool Init(unsigned int uiMapInx);
    char Load(unsigned __int8 byDayID, _worlddb_guild_battle_reserved_schedule_info *kInfo);
    char Clear();
    unsigned __int8 GetMaxPage();
    CReservedGuildSchedulePage *Find(unsigned int dwGuildSerial);
    void Send(int n, unsigned int dwVer, unsigned __int8 byPage, CReservedGuildSchedulePage *pkSelfInfoPage);
    void Flip();

    unsigned int m_uiMapInx;
    unsigned __int8 m_byMaxPage;
    CReservedGuildSchedulePage m_kList[6];
  };

  class CReservedGuildScheduleDayGroup
  {
  public:
    CReservedGuildScheduleDayGroup();
    bool Init(unsigned int uiMapCnt);
    bool Load(unsigned __int8 byDayID, unsigned int uiMapInx, _worlddb_guild_battle_reserved_schedule_info *kInfo);
    char Clear();
    CReservedGuildSchedulePage *Find(unsigned int dwGuildSerial);
    void Send(
      char byDayID,
      unsigned int uiMapID,
      unsigned int n,
      unsigned int dwVer,
      unsigned __int8 byPage,
      unsigned int dwGuildSerial);
    void Flip();

    unsigned int m_uiMapCnt;
    unsigned __int8 m_byToday;
    unsigned __int8 m_byTommorow;
    CReservedGuildScheduleMapGroup *m_pkList;
  };

  class CGuildBattleReservedScheduleListManager
  {
  public:
    static CGuildBattleReservedScheduleListManager *Instance();
    CGuildBattleReservedScheduleListManager();
    bool Init();
    char Load(int iCurDay, unsigned int uiOldMapCnt, int iToday, int iTomorrow);
    char LoadTodaySchedule();
    char LoadTomorrowSchedule();
    char UpdateTodaySchedule(unsigned int uiMapID);
    char UpdateTomorrowSchedule(unsigned int uiMapID);
    void Flip();
    void Send(
      unsigned int uiMapID,
      int n,
      unsigned int dwVer,
      unsigned __int8 byDay,
      unsigned __int8 byPage,
      unsigned int dwGuildSerial);
    void PushDQS(unsigned int dwMapID, unsigned int dwSLID);
    bool UpdateReservedShedule(unsigned int dwSLID, _worlddb_guild_battle_reserved_schedule_info *byOutData);
    void UpdateTomorrowComplete(unsigned int dwMapID, _worlddb_guild_battle_reserved_schedule_info *pLoadData);

    static CGuildBattleReservedScheduleListManager *ms_Instance;
    unsigned int m_uiMapCnt;
    CReservedGuildScheduleDayGroup m_kList[2];
    CReservedGuildScheduleDayGroup *m_pkToday;
    CReservedGuildScheduleDayGroup *m_pkTomorrow;
  };

  class CGuildBattleScheduler
  {
  public:
    static CGuildBattleScheduler *Instance();
    bool Init();
    static void Destroy();
    char UpdateClearGuildBattleScheduleDayInfo(unsigned int dwStartSLID, unsigned int dwEndSLID);

    static CGuildBattleScheduler *ms_Instance;
  };

  class CCurrentGuildBattleInfoManager
  {
  public:
    static CCurrentGuildBattleInfoManager *Instance();
    CCurrentGuildBattleInfoManager();
    bool Init();
    void Send(unsigned int n, unsigned int uiMapID);
    unsigned __int8 GetLeftTime(unsigned int uiMapID);
    void UpdateGoalCnt(unsigned int uiMapID, unsigned __int8 byColorInx, unsigned int dwGoalCnt);
    void UpdateScore(unsigned int uiMapID, unsigned __int8 byColorInx, unsigned int dwScore);

    static CCurrentGuildBattleInfoManager *ms_Instance;
    bool m_bInit;
    unsigned int m_uiMapCnt;
    bool *m_pbUpdate;
    _guild_battle_current_battle_info_result_zocl *m_pkInfo;
  };

  class CPossibleBattleGuildListManager
  {
  public:
    static CPossibleBattleGuildListManager *Instance();
    CPossibleBattleGuildListManager();
    bool Init();
    char Load();
    void SendFirst(int n, unsigned __int8 byRace);
    unsigned __int8 SendInfo(unsigned int n, unsigned __int8 byRace, unsigned __int8 byPage, unsigned int dwVer);
    void UpdateGuildList();
    bool MakePage(unsigned __int8 byRace, unsigned __int8 ucPage, unsigned __int16 *wLastGuildInx);

    static CPossibleBattleGuildListManager *ms_Instance;
    bool m_bInit;
    unsigned int *m_pdwVer;
    unsigned __int8 *m_pMaxPage;
    _possible_battle_guild_list_result_zocl **m_ppkList;
  };
} // namespace GUILD_BATTLE
