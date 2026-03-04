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
struct _qry_case_guild_battel_result_log;

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
    CGuildBattle();
    virtual ~CGuildBattle();

    virtual int GetObjType();
  };

  class CGuildBattleState
  {
  public:
    CGuildBattleState();
    virtual ~CGuildBattleState();

    virtual int Enter(CGuildBattle *pkBattle);
    virtual int Loop(CGuildBattle *pkBattle);
    virtual int Fin(CGuildBattle *pkBattle);
    virtual ATL::CTimeSpan *GetTerm(ATL::CTimeSpan *result);
    void Log(char *szMsg);
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
    virtual ~CGuildBattleStateList();
    virtual void SetNextState();
    void Clear();
    void SetWait();
    void SetReady();
    bool IsProc();
    void Process(CGuildBattle *pkBattle);
    void Advance(int iAdvance);
    int Next(bool bForce);
    int CheckLoop();
    ATL::CTimeSpan *GetTerm(ATL::CTimeSpan *result);
    bool IsEmpty();
    int Goto();
    void ForceNext();
    char GotoState(int iState);

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
    ~CNormalGuildBattleState() override;
    int Loop(CGuildBattle *pkBattle) override;
    void Log(CNormalGuildBattle *pkBattle, char *szFormat, ...);
  };

  class CNormalGuildBattleStateNotify : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateNotify();
    ~CNormalGuildBattleStateNotify() override;
  };

  class CNormalGuildBattleStateReady : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateReady();
    ~CNormalGuildBattleStateReady() override;
  };

  class CNormalGuildBattleStateCountDown : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateCountDown();
    ~CNormalGuildBattleStateCountDown() override;
  };

  class CNormalGuildBattleStateRound : public CGuildBattleState
  {
  public:
    CNormalGuildBattleStateRound();
    ~CNormalGuildBattleStateRound() override;
    int Loop(CGuildBattle *pkBattle) override;
    void Log(CNormalGuildBattle *pkBattle, char *szFormat, ...);
  };

  class CNormalGuildBattleStateRoundStart : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundStart();
    ~CNormalGuildBattleStateRoundStart() override;
    int Loop(CGuildBattle *pkBattle) override;

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundProcess : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundProcess();
    ~CNormalGuildBattleStateRoundProcess() override;
    int Loop(CGuildBattle *pkBattle) override;

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundReturnStartPos : public CNormalGuildBattleStateRound
  {
  public:
    CNormalGuildBattleStateRoundReturnStartPos();
    ~CNormalGuildBattleStateRoundReturnStartPos() override;
    int Loop(CGuildBattle *pkBattle) override;

    CMyTimer *m_pkTimer;
  };

  class CNormalGuildBattleStateRoundList : public CGuildBattleStateList
  {
  public:
    CNormalGuildBattleStateRoundList();
    ~CNormalGuildBattleStateRoundList() override;
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
    ~CNormalGuildBattleStateInBattle() override;
    int Loop(CGuildBattle *pkBattle) override;
    void SetBattleTime(ATL::CTimeSpan kTime);
    bool IsInBattleRegenState();
    bool SetGotoRegenState();

    ATL::CTimeSpan m_kInBattleTime;
    CNormalGuildBattleStateRoundList m_kRountStateList;
  };

  class CNormalGuildBattleStateDivide : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateDivide();
    ~CNormalGuildBattleStateDivide() override;
  };

  class CNormalGuildBattleStateReturn : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateReturn();
    ~CNormalGuildBattleStateReturn() override;
  };

  class CNormalGuildBattleStateFin : public CNormalGuildBattleState
  {
  public:
    CNormalGuildBattleStateFin();
    ~CNormalGuildBattleStateFin() override;
  };

  class CNormalGuildBattleStateList : public CGuildBattleStateList
  {
  public:
    CNormalGuildBattleStateList();
    ~CNormalGuildBattleStateList() override;
    void SetNextState() override;
    bool IsReadyOrCountState();
    bool IsInBattle();
    bool IsInBattleRegenState();
    bool SetGotoRegenState();
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
    static void Destroy();
    CNormalGuildBattleStateListPool();
    ~CNormalGuildBattleStateListPool();
    bool Init();
    CNormalGuildBattleStateList *Get(unsigned int dwID);

    static CNormalGuildBattleStateListPool *ms_Instance;
    unsigned int m_dwMaxCount;
    CNormalGuildBattleStateList *m_pkStateList;
  };

  class CNormalGuildBattleField
  {
  public:
    CNormalGuildBattleField();
    ~CNormalGuildBattleField();

    bool Init(unsigned int uiMapInx);
    void Destroy();
    char CreateFieldObject();
    void DestroyFieldObject();
    char ClearRegen();
    char ClearBall();
    char CheckIsInTown();
    bool CheckBallTakeLimitTime();
    bool Start(unsigned __int8 byStartPos, CPlayer *pkPlayer);
    char MoveStartPos(unsigned __int8 byStartPos, unsigned __int8 byMapOutType, CPlayer *pkPlayer);
    CGravityStone *GetStone();
    CGravityStoneRegener *GetRegener(int iInx);
    CCircleZone *GetCircleZone(int iInx);
    unsigned __int8 DropBall(CPlayer *pkPlayer);
    unsigned __int8 TakeBall(int iPortalInx, CPlayer *pkPlayer);
    int CheatRegenStone(unsigned int uiPos);
    int CheatRegenStone(CPlayer *pkPlayer);
    unsigned __int8 CheatTakeStone(int iPortalInx, CPlayer *pkPlayer);
    unsigned __int8 CheatGetStone(CPlayer *pkPlayer);
    char CheatForceTakeStone(CPlayer *pkPlayer);
    void CheatDestroyStone();
    void GetPortalIndexInfo(int *iRedPortalInx, int *iBluePortalInx, int *piRegenPortalInx);
    unsigned __int8 CheatDropStone(CPlayer *pkPlayer);
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
    static void Destroy();
    CNormalGuildBattleFieldList();
    ~CNormalGuildBattleFieldList();
    bool Init();
    bool InitUseField(unsigned __int8 byRace, char *szKeyName, char *szStrBuff, char **szParseBuff);
    unsigned int GetMapCnt();
    char GetMapInx(unsigned __int8 byRace, unsigned int dwMapCode, unsigned int *dwMapInx);
    __int64 GetMapInxList(unsigned __int8 byRace, unsigned __int8 *pbyInxList);
    CNormalGuildBattleField *GetField(unsigned int dwMapID);
    CNormalGuildBattleField *GetField(unsigned __int8 byRace, unsigned int dwMapCode);
    CGravityStone *GetStone(int iInx);
    CGravityStoneRegener *GetRegener(int iInx);
    CCircleZone *GetCircleZone(int iInx);

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
    ~CNormalGuildBattleLogger();
    bool Init();
    void CreateLogFile(char *szLogName);
    void Log(const char *fmt, ...);

    CLogFile *m_pkLogger;
  };

  class CNormalGuildBattleGuildMember
  {
  public:
    CNormalGuildBattleGuildMember();
    ~CNormalGuildBattleGuildMember();
    void Clear();
    bool IsEmpty();
    bool IsExist();
    bool IsEnableStart();
    unsigned int GetSerial();
    unsigned __int16 GetIndex();
    CPlayer *GetPlayer();
    void AddKillCnt();
    void StockOldInfo();
    void SetReStartFlag();
    void SetBattleState(bool bFlag, unsigned __int8 byColorInx);
    void Send(unsigned __int8 *byType, char *pSend, unsigned __int16 uiSize);
    void CleanUpBattle();
    void NetClose();
    void ReturnStartPos();
    void ReturnBindPos();
    void PushDQSPvpPoint(int dwPvpPoint);

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
    ~CNormalGuildBattleGuild();
    void Clear();
    void SetGuild(CGuild *pkGuild);
    CGuild *GetGuild();
    unsigned int GetGuildSerial();
    char *GetGuildName();
    unsigned __int8 GetGuildRace();
    unsigned __int8 GetColorInx();
    char IsJoinMember(unsigned int dwSerial);
    __int64 GetMember(unsigned int dwSerial);
    CNormalGuildBattleGuildMember *GetMemberPtr(unsigned int dwSerial);
    CPlayer *GetMemberPlayer(unsigned int dwSerial);
    unsigned int GetScore();
    void UpdateScore();
    int Kill(CNormalGuildBattleGuildMember *pkSrcMember, CNormalGuildBattleGuildMember *pkDestMember);
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
    void NotifyLeftMinuteBeforeStart(char byLeftMin);
    void SendRegenBall(int iPortalInx);
    void SendGetGravityStone(CNormalGuildBattleGuild *pkTakeGuild, CPlayer *pkPlayer, int iTakePortalInx);
    void SendDeleteNotifyPositionMember(int iMemberInx);
    void MoveMap(unsigned int uiID, CNormalGuildBattleField *pkField);
    char SetReStartFlag(unsigned int dwSerial);
    char MoveMember(int iMember, unsigned int uiID, CNormalGuildBattleField *pkField, CNormalGuildBattleLogger *kLogger);
    char NetClose(bool bInGuildBattle, unsigned int dwSerial, CNormalGuildBattleLogger *kLogger);
    bool IsMember(unsigned int dwSerial);
    void LeaveGuild(unsigned int dwSerial, bool bInGuildBattle, CNormalGuildBattleLogger *kLogger);
    char ReturnStartPosAll(CNormalGuildBattleField *pkField);
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
    ~CNormalGuildBattle() override;
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
    bool IsProc();
    void Process();
    void SetReadyState();
    unsigned __int8 Join(unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 Start(CPlayer *pkPlayer, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 ReStart(CPlayer *pkPlayer, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 DropGravityStone(unsigned int dwCharacSerial);
    int Kill(unsigned int dwSrcCharacSerial, unsigned int dwDestCharacSerial);
    void AskJoin(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    void LogIn(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    unsigned __int8 NetClose(unsigned int dwCharacSerial, CPlayer *pkPlayer);
    unsigned __int8 LeaveGuild(CPlayer *pkPlayer);
    void NotifyBeforeStart();
    void NotifyBattleResult(char byResult);
    void SendKillInform();
    void SendWebBattleStartInfo();
    void SendWebBattleEndInfo();
    void SendGoalMsg(bool b1P, char *wszGuildName, CPlayer *pkPlayer);
    void GuildBattleResultLogNotifyWeb(const _qry_case_guild_battel_result_log *Sheet);
    void NotifyDestoryBall(unsigned int dwOwnerSerial);
    void PushDQSWinLoseRank();
    void PushDQSDrawRank();
    void GuildBattleResultLogPushDBLog(
      _qry_case_guild_battel_result_log *sheet,
      CNormalGuildBattleGuildMember *pkTopGoalMember,
      CNormalGuildBattleGuildMember *pkTopKillMember);
    unsigned int GetID();
    CNormalGuildBattleGuild *GetRed();
    CNormalGuildBattleGuild *GetBlue();
    CNormalGuildBattleField *GetField();
    CNormalGuildBattleLogger *GetLogger();
    bool SetGotoRegenStart();
    void NotifyPassGravityStoneLimitTime();
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
    static void Destroy();
    CNormalGuildBattleManager();
    ~CNormalGuildBattleManager();
    bool Init();
    bool Load(int iCurDay, unsigned int uiOldMapCnt, int iToday, unsigned int iTodayDayID, int iTomorrow, unsigned int iTomorrowDayID);
    char Load(bool bToday, unsigned int uiDayID, CNormalGuildBattle **ppkStart);
    char LoadDBGuildBattleInfo(unsigned int dwStartID, _worlddb_guild_battle_info *kInfo);
    char AddDefaultDBRecord();
    void Loop();
    void Clear();
    void Clear(CNormalGuildBattle **ppkStart);
    void Flip();
    void DoDayChangedWork();
    void SetReadyState(CNormalGuildBattle **ppkStart);
    void SetNextEvent();
    void Join(unsigned int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial);
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
    unsigned __int8 DropGravityStone(unsigned int dwGuildSerial, unsigned int dwCharacSerial);
    int Kill(unsigned int dwGuildSerial, unsigned int dwSrcCharacSerial, unsigned int dwDestCharacSerial);
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
    ~CGuildBattleSchedule();
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
    int Check();
    int Process();
    bool IsWait();
    void SetProcState();
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
    ~CGuildBattleSchedulePool();
    bool Init(unsigned int uiMapCnt);
    CGuildBattleSchedule *Get(unsigned int dwSID);
    CGuildBattleSchedule *Get(unsigned int uiSLID, unsigned int dwStartInx);
    CGuildBattleSchedule *GetRef(unsigned int dwSID);
    unsigned int GetSID(unsigned int uiSLID, unsigned int dwStartInx);
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
    ~CGuildBattleReservedSchedule();
    void Clear();
    char Clear(unsigned int dwID);
    unsigned int GetID();
    bool IsDone();
    bool Loop();
    char Next();
    char CheckNextEvent(int iRet);
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
    ~CGuildBattleReservedScheduleMapGroup();
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
    bool IsDone();
    char Loop();
    CGuildBattleSchedule *UpdateUseFlag(unsigned int uiMapID, unsigned int dwID);
    unsigned int GetDayID();
    char GetSLID(unsigned int uiMapID, unsigned int *uiSLID);
    char CleanUpDanglingReservedSchedule();
    unsigned int GetCurScheduleID(unsigned int uiMapID);
    void PushDQSClear();
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
    ~CGuildBattleScheduleManager();
    bool Init(unsigned int uiMapCnt);
    void Loop();
    void UpdateDayChangedWork();
    int IsDayChanged();
    void Clear();
    void Flip();
    void SetNextEvnet();
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
    static void Destroy();
    CGuildBattleLogger();
    ~CGuildBattleLogger();
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
    ~CGuildBattleRewardItemManager();
    bool Init();
    const CGuildBattleRewardItem *Give(CPlayer *pkPlayer);

    static CGuildBattleRewardItemManager *ms_Instance;
    std::vector<CGuildBattleRewardItem> m_kItem;
  };

  class CGuildBattleRankManager
  {
  public:
    static CGuildBattleRankManager *Instance();
    static void Destroy();
    CGuildBattleRankManager();
    ~CGuildBattleRankManager();
    bool Init();
    void CleanUp();
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
    ~CReservedGuildSchedulePage();
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
    ~CReservedGuildScheduleMapGroup();
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
    ~CReservedGuildScheduleDayGroup();
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
    static void Destroy();
    CGuildBattleReservedScheduleListManager();
    ~CGuildBattleReservedScheduleListManager();
    bool Init();
    char Load(int iCurDay, unsigned int uiOldMapCnt, int iToday, int iTomorrow);
    char LoadTodaySchedule();
    char LoadTomorrowSchedule();
    char UpdateTodaySchedule(unsigned int uiMapID);
    char UpdateTomorrowSchedule(unsigned int uiMapID);
    void Clear();
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
    CGuildBattleScheduler();
    ~CGuildBattleScheduler();

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
    static void Destroy();
    CCurrentGuildBattleInfoManager();
    ~CCurrentGuildBattleInfoManager();
    void CleanUp();
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
    static void Destroy();
    CPossibleBattleGuildListManager();
    ~CPossibleBattleGuildListManager();
    bool Init();
    char Load();
    void Clear();
    void DoDayChangedWork();
    void SendFirst(int n, unsigned __int8 byRace);
    void SendErrorResult(unsigned int n, char byRet);
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
