#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

/* 1536 */
class __cppobj __declspec(align(8)) CUserDB
{
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
  virtual ~CUserDB() = default;
};

