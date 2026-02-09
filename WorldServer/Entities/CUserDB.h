#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMainThread.h"
#include "CMgrAccountLobbyHistory.h"
#include "MOVE_LOBBY_DELAY.h"

struct _EXIT_ALTER_PARAM;

/* 1536 */
class __cppobj __declspec(align(8)) CUserDB
{
public:
  static int s_nLoginNum;
  static CLogFile s_logAvatorDB;
  static CMgrAccountLobbyHistory s_MgrLobbyHistory;
  static _MOVE_LOBBY_DELAY s_MoveLobbyDelay;

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
  __declspec(align(4)) _BILLING_INFO m_BillingInfo;
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
  void ParamInit();
  bool Update_Map(unsigned __int8 map, float *pos);
  char Update_Bind(char *pszMapCode, char *pDummyCode, bool bUpdate);
  void Update_PvpPointLeak(long double dValue);
  void Update_LastAttBuff(bool bSet);
  bool Update_AlterPvPPoint(long double dNewPoint);
  bool Update_ItemAdd(
    unsigned __int8 storage,
    unsigned __int8 slot,
    const _STORAGE_LIST::_db_con *pItem,
    bool bUpdate);
  bool Update_ItemDelete(unsigned __int8 storage, unsigned __int8 slot, bool bUpdate);
  char Update_ItemDur(unsigned __int8 storage, unsigned __int8 slot, unsigned __int64 amount, bool bUpdate);
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
  bool Update_Stat(unsigned __int8 byStatIndex, unsigned int dwNewCum, bool bUpdate);
  bool Update_UnitData(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData);
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
  void DelPostData(unsigned int dwIndex);
  char Update_Param(_EXIT_ALTER_PARAM *pCon);
  void ForceCloseCommand(unsigned __int8 byKickType, unsigned int dwPushIP, bool bSlow, const char *pszCause);
  void ClearBillingData();
  void SetBillingData(_BILLING_INFO *pBillingInfo);
  void SetBillingData(char *szCMSCode, __int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate);
  int GetBillingType();
  void SetBillingNoLogout(bool bNoLogout);
  void SendMsg_BillingInfo();
  void SetRemainTime(int lRemainTime);
  void UILockInfo_Init(char *pMsg);
  void UILockInfo_Update(char *pMsg);
  void SetChatLock(bool bLock);
  void SetWorldCLID(unsigned int dwSerial, unsigned int *pipAddr);
  void SendMsgAccount_UILockRefresh_Update();
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
  static char CheckDQSLoadCharacterData(_AVATOR_DATA *pData);
  virtual ~CUserDB() = default;
};

CUserDB *SearchAvatorWithName(CUserDB *pList, int nMax, char *pwszName);

