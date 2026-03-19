#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMainThread.h"
#include "CMgrAccountLobbyHistory.h"
#include "MOVE_LOBBY_DELAY.h"

#include <cstddef>

struct _EXIT_ALTER_PARAM;
struct _ITEMCOMBINE_DB_BASE;
struct _uilock_init_result_acwr;
struct _uilock_update_result_acwr;

/* 1536 */
class  CUserDB
{
public:
  CUserDB();
  virtual ~CUserDB();

  static int s_nLoginNum;
  static CLogFile s_logAvatorDB;
  static CMgrAccountLobbyHistory s_MgrLobbyHistory;
  static _MOVE_LOBBY_DELAY s_MoveLobbyDelay;
  static bool IsValidBaseShape(unsigned int baseShape);

  _GLBID m_gidGlobal;
  _CLID m_idWorld;
  unsigned int m_dwIP;
  unsigned int m_dwTotalPlayMin;
  char m_szAccountID[13];
  unsigned int m_dwAccountSerial;
  unsigned int m_ipAddress;
  unsigned __int8 m_byUserDgr;
  unsigned __int8 m_bySubDgr;
  char m_wszAvatorName[17];
  char m_aszAvatorName[17];
  unsigned int m_dwSerial;
  unsigned __int8 m_byNameLen;
  _REGED m_RegedList[3];
  _AVATOR_DATA m_AvatorData;
  _AVATOR_DATA m_AvatorData_bk;
  _NOT_ARRANGED_AVATOR_DB m_NotArrangedChar[50];
  unsigned int m_dwArrangePassCase0[50];
  bool m_bActive;
  bool m_bField;
  bool m_bWndFullMode;
  bool m_bDBWaitState;
  _DB_QRY_SYN_DATA *m_pDBPushData;
  bool m_bChatLock;
  _SYNC_STATE m_ss;
  unsigned int m_dwMessengerKey[4];
  unsigned int m_dwOperLobbyTime;
  bool m_bCreateTrunkFree;
  CMyTimer m_tmrCheckPlayMin;
  bool m_bDataUpdate;
  unsigned int m_dwTermContSaveTime;
  unsigned int m_dwLastContSaveTime;
  bool m_bNoneUpdateData;
  _BILLING_INFO m_BillingInfo;
  bool m_bBillingNoLogout;
  int m_nTrans;
  CRadarItemMgr m_RadarItemMgr;
  unsigned __int8 m_byUILock;
  bool m_bUILock_Updated;
  unsigned __int8 m_byUILock_InitFailCnt;
  unsigned __int8 m_byUILock_FailCnt;
  char m_szUILock_PW[13];
  char m_szAccount_PW[13];
  unsigned __int8 m_byUILock_HintIndex;
  char m_uszUILock_HintAnswer[17];
  unsigned __int8 m_byUILock_InitFindPassFailCount;
  unsigned __int8 m_byUILockFindPassFailCount;
  unsigned int m_dwRequestMoveCharacterSerialList[3];
  unsigned int m_dwTournamentCharacterSerialList[3];
  char m_szLobbyHistoryFileName[64];
  void Init(unsigned __int16 index);
  void DummyCreate(unsigned int dwSerial);
  void ParamInit();
  bool Update_Map(unsigned __int8 map, float *pos);
  bool Update_Bind(char *pszMapCode, char *pDummyCode, bool bUpdate);
  bool Update_AddBuddy(unsigned __int8 bySlotIndex, unsigned int dwAdderSerial, char *pwszAdderName);
  bool Update_DelBuddy(unsigned __int8 bySlotIndex);
  bool Update_UserGetScaner(unsigned __int16 wScanerCnt, unsigned __int16 wBattleTime);
  bool Update_UserPlayTime(unsigned int dwAccPlayTime);
  bool Update_LinkBoardSlot(unsigned __int8 bySlot, unsigned __int8 byLinkCode, unsigned __int16 wIndex);
  bool Update_LinkBoardLock(unsigned __int8 byLBLock);
  bool Update_WindowInfo(
    unsigned int *pdwSkill,
    unsigned int *pdwForce,
    unsigned int *pdwChar,
    unsigned int *pdwAnimus,
    unsigned int dwInven,
    unsigned int *pdwInvenBag);
  bool Update_Macro(char *pBuf);
  void Update_BossCryMsg(unsigned __int8 bySlot, char *pwszCryMsg);
  bool Update_BagNum(unsigned __int8 bagnum);
  void CalcRadarDelay();
  void SetRadarDelay(unsigned int dwDelay);
  void Update_PvpPointLeak(long double dValue);
  void Update_LastAttBuff(bool bSet);
  bool Update_UserFatigue(unsigned int dwFatigue);
  bool Update_UserTLStatus(unsigned __int8 byStatus);
  bool Update_RaceVoteInfoInit();
  unsigned int GetActPoint(unsigned __int8 byCode);
  unsigned int *GetPtrActPoint();
  void SetActPoint(unsigned __int8 byCode, unsigned int dwLeftPoint);
  bool Update_User_Action_Point(unsigned __int8 byActionCode, unsigned int dwPoint);
  bool InitClass(char *pszClassCode);
  bool Update_Class(char *pszClassCode, unsigned __int8 byHistoryRecordNum, unsigned __int16 wHistoryClassIndex);
  void WriteLog_ChangeClassAfterInitClass(unsigned __int8 byType, char *szPrevClass);
  void StartFieldMode();
  void Update_PotionNextUseTime(unsigned __int8 byPotionClass, unsigned int dwNextUseTime);
  bool Update_AlterPvPPoint(long double dNewPoint);
  bool Update_AlterPvPCashBag(long double dNewPoint);
  bool Update_ItemAdd(
    unsigned __int8 storage,
    unsigned __int8 slot,
    const _STORAGE_LIST::_db_con *pItem,
    bool bUpdate);
  bool Update_ItemDelete(unsigned __int8 storage, unsigned __int8 slot, bool bUpdate);
  bool Update_ItemSlot(unsigned __int8 storage, unsigned __int8 slot, unsigned __int8 clientpos);
  bool Update_ItemUpgrade(unsigned __int8 storage, unsigned __int8 slot, unsigned int upg, bool bUpdate);
  bool Update_ItemDur(unsigned __int8 storage, unsigned __int8 slot, unsigned __int64 amount, bool bUpdate);
  bool Update_Money(unsigned int dalant, unsigned int gold);
  bool Update_SFContDelete(unsigned __int8 byContCode, unsigned __int8 bySlotIndex);
  bool Update_SFContInsert(
    unsigned __int8 byContCode,
    unsigned __int8 bySlotIndex,
    unsigned __int8 byEffectCode,
    unsigned __int16 wEffectIndex,
    unsigned __int8 byLv,
    unsigned __int16 wDurSec);
  bool Update_SFContUpdate(unsigned __int8 byContCode, unsigned __int8 bySlotIndex, unsigned __int16 wTime);
  bool Update_TakeLastMentalTicket(unsigned int dwMentalTicket);
  bool Update_TrunkSlotNum(unsigned __int8 bySlotNum);
  bool Update_ExtTrunkSlotNum(unsigned __int8 byExtSlotNum);
  bool Update_Stat(unsigned __int8 byStatIndex, unsigned int dwNewCum, bool bUpdate);
  bool Update_UnitInsert(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData);
  bool Update_UnitDelete(unsigned __int8 bySlotIndex);
  bool Update_UnitData(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData);
  bool Update_QuestInsert(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData);
  bool Update_QuestDelete(unsigned __int8 bySlotIndex);
  bool Update_QuestUpdate(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData, bool bUpdate);
  bool Update_NPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData);
  void Update_MaxLevel(unsigned __int8 byMaxLevel);
  bool Update_Level(unsigned __int8 lv, long double exp);
  bool Update_LossExp(long double dLossExp);
  bool Update_Exp(long double exp);
  bool Setting_Class(char *pszClassCode);
  void WriteLog_Level(unsigned __int8 byLv);
  void WriteLog_CharSelect();
  bool Update_CombineExResult_Push(_ITEMCOMBINE_DB_BASE *pItemCombineDB_IN);
  bool Update_CombineExResult_Pop();
  bool Update_CuttingEmpty();
  bool Update_CuttingPush(unsigned __int8 resnum, _CUTTING_DB_BASE::_LIST *plist);
  bool Update_CuttingTrans(unsigned __int16 wResItemIndex, unsigned __int16 wLeftAmt);
  bool Update_StartNPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY *pHisData);
  bool Update_TrunkHint(unsigned __int8 byHintIndex, char *pwszHintAnswer);
  bool Update_TrunkMoney(long double dGold, long double dDalant);
  bool Update_TrunkPassword(char *pwszPassword);
  bool Update_UserVoteData();
  bool Update_AutoTradeAllClear();
  bool Update_TakeLastCriTicket(unsigned int dwCriTicket);
  void SetDBPostData(
    unsigned int n,
    unsigned int dwSerial,
    int nNumber,
    unsigned __int8 byState,
    int nKey,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    bool bUpdateIndex,
    unsigned __int64 lnUID);
  void Update_Post(
    unsigned int n,
    unsigned int dwSerial,
    int nNumber,
    unsigned __int8 byState,
    int nKey,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    unsigned __int64 lnUID);
  void SetNewDBPostData(
    unsigned int n,
    unsigned int dwSerial,
    int nNumber,
    unsigned __int8 byState,
    char *wszSendName,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    int nKey,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    unsigned __int64 lnUID);
  bool Update_DelPost(unsigned int dwSerial, unsigned int nIndex);
  bool IsReturnPostUpdate();
  void Update_ReturnPost(unsigned int dwSerial);
  void DelPostData(unsigned int dwIndex);
  bool Update_Param(_EXIT_ALTER_PARAM *pCon);
  bool Update_CopyAll(_AVATOR_DATA *pSrc);
  void ForceCloseCommand(unsigned __int8 byKickType, unsigned int dwPushIP, bool bSlow, const char *pszCause);
  void Inform_For_Exit_By_FireguardBlock();
  void ClearBillingData();
  void SetBillingData(_BILLING_INFO *pBillingInfo);
  void SetBillingData(char *szCMSCode, __int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate);
  int GetBillingType();
  void SetBillingNoLogout(bool bNoLogout);
  void SendMsg_BillingInfo();
  void SetRemainTime(int lRemainTime);
  void UILockInfo_Init(const _uilock_init_result_acwr *request);
  void UILockInfo_Update(const _uilock_update_result_acwr *request);
  void SendMsg_Inform_UILock();
  void SetChatLock(bool bLock);
  void SetWorldCLID(unsigned int dwSerial, unsigned int *pipAddr);
  void SendMsgAccount_UILockRefresh_Update();
  bool Enter_Account(unsigned int dwAccountSerial, unsigned int dwIP, unsigned int dwProtocolVer, unsigned int *pdwMasterKey);
  bool Lobby_Char_Request();
  bool Reged_Char_Request();
  bool Insert_Char_Request(
    char *pwszCharName,
    unsigned __int8 bySlotIndex,
    unsigned __int8 byRaceSexCode,
    char *pszClassCode,
    unsigned int dwBaseShape);
  bool Delete_Char_Request(unsigned __int8 bySlotIndex);
  bool Select_Char_Request(unsigned __int8 bySlotIndex);
  bool Alive_Char_Request(
    unsigned __int8 byCase,
    unsigned int dwSerial,
    char *pwszName,
    unsigned __int8 bySlotIndex);
  unsigned __int8 IsExistRequestMoveCharacterList(unsigned int dwCharSerial);
  bool DataValidCheckRevise(_AVATOR_DATA *pData, bool *pDataUpdated);
  bool FirstSettingData();
  bool Update_PlayTime(unsigned int dwTotalTimeMin);
  void TotalPlayMinCheck();
  bool UpdateContUserSave(bool bDirect);
  void Insert_Char_Complete(unsigned __int8 byRetCode, _REGED_AVATOR_DB *pInsertData);
  void Reged_Char_Complete(unsigned __int8 byRetCode, _REGED *pRegedList, _NOT_ARRANGED_AVATOR_DB *pArrangedList);
  void Delete_Char_Complete(unsigned __int8 byRetCode, unsigned __int8 bySlotIndex);
  void Select_Char_Complete(
    unsigned __int8 byRetCode,
    _AVATOR_DATA *pLoadData,
    bool *pbAddItem,
    unsigned int dwAddDalant,
    unsigned int dwAddGold,
    unsigned int dwCheckSum,
    bool *pbTrunkAddItem,
    unsigned __int8 byTrunkOldSlot,
    long double dTrunkOldDalant,
    long double dTrunkOldGold,
    bool bCreateTrunkFree,
    bool *pbExtTrunkAddItem,
    unsigned __int8 byExtTrunkOldSlot);
  void Lobby_Char_Complete(unsigned __int8 byRetCode);
  void Cont_UserSave_Complete(unsigned __int8 byResult, _AVATOR_DATA *pAvatorData);
  void Alive_Char_Complete(
    unsigned __int8 byRetCode,
    unsigned __int8 byCase,
    unsigned int dwSerial,
    _REGED *pAliveAvator);
  _AVATOR_DATA *IsContPushBefore();
  void Exit_Account_Request();
  void Exit_Account_Complete(unsigned __int8 byRetCode);
  static void ReRangeClientIndex(_AVATOR_DATA *pData);
  static bool CheckDQSLoadCharacterData(_AVATOR_DATA *pData);
  static void OnLoop_Static();
};

CUserDB *SearchAvatorWithName(CUserDB *pList, int nMax, char *pwszName);

extern const char wszNonMakeName_0[3][17];
extern const char wszNonMakeName_1[3][17];
extern const char wszNonMakeName[3][17];
extern const char wszGMCmp[];
extern size_t nGMCmpLen;


