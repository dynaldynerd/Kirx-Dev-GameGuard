#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"
#include "CPostData.h"
#include "StorageList.h"

class CPlayer;
struct _STORAGE_POS_INDIV;
struct _qry_case_post_list_regi;
struct _qry_case_post_send;
struct _qry_case_post_serial_check;

class  CPostSystemManager
{
public:
  static CPostSystemManager *ms_Instance;

  CPostSystemManager();
  ~CPostSystemManager();

  static CPostSystemManager *Instace();
  static void Destroy();
  bool Init();
  void Loop();
  bool Load();
  bool InitLogger();
  bool UpdateDisappearOwnerRecord();
  bool InsertDefaultPSRecord();
  bool PostRegistryLoad();
  unsigned __int8 UpdateRegist(_qry_case_post_list_regi *pData);
  unsigned __int8 PostSend(_qry_case_post_send *pData);
  unsigned __int8 PostReceiverCheck(_qry_case_post_serial_check *pData);
  unsigned __int8 CheckRegister(
    CPlayer *pOne,
    _STORAGE_POS_INDIV *pItemInfo,
    unsigned int dwGold,
    _STORAGE_LIST::_db_con **pItem);
  bool PostSendRequest(
    CPlayer *pOne,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    _STORAGE_POS_INDIV *pItemInfo,
    unsigned int dwGold,
    unsigned __int8 byRace);
  void CompleteRegist(_qry_case_post_list_regi *pData);
  void CompleteSend(_qry_case_post_send *pData);
  void CompletePostReceiverCheck(_qry_case_post_serial_check *pData);
  void SetNextWriteTime();
  void Log(const char *fmt, ...);
  void Log(const wchar_t *fmt, ...);

  unsigned __int8 m_byRegiNum;
  unsigned __int8 m_byProcNum;
  CMyTimer m_tmrRegiTime;
  CMyTimer m_tmrProcTime;
  CNetIndexList m_listEmpty;
  CNetIndexList m_listRegist;
  CNetIndexList m_listProc;
  CLogFile *m_pkLogger;
  CPostData *m_PostData;
  __int64 m_tNextWrite;
  int m_nPostProcCountPerDay;
  int m_nPostReturnCountPerDay;
};
