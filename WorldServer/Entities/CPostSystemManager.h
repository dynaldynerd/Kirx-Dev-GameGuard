#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"
#include "CPostData.h"
#include "StorageList.h"

class CPlayer;
struct _STORAGE_POS_INDIV;

class __cppobj CPostSystemManager
{
public:
  static CPostSystemManager *Instace();
  bool Init();
  bool Load();
  bool InitLogger();
  char UpdateDisappearOwnerRecord();
  char InsertDefaultPSRecord();
  char PostRegistryLoad();
  unsigned __int8 UpdateRegist(char *pData);
  unsigned __int8 PostSend(char *pData);
  unsigned __int8 PostReceiverCheck(char *pData);
  unsigned __int8 CheckRegister(
    CPlayer *pOne,
    _STORAGE_POS_INDIV *pItemInfo,
    unsigned int dwGold,
    _STORAGE_LIST::_db_con **pItem);
  char PostSendRequest(
    CPlayer *pOne,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    _STORAGE_POS_INDIV *pItemInfo,
    unsigned int dwGold,
    unsigned __int8 byRace);
  void CompleteRegist(char *pData);
  void CompleteSend(char *pData);
  void CompletePostReceiverCheck(char *pData);
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
