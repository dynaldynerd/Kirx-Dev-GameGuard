#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

namespace RACE_BOSS_MSG
{
  struct CMsg
  {
    unsigned int m_uiState;
    unsigned int m_dwID;
    unsigned int m_dwSerial;
    char m_wszName[17];
    char m_wszMsg[49];
    unsigned int m_dwSendTime;
    unsigned int m_dwWebSendDBID;

    bool Save(unsigned __int8 ucRace);
  };

  class CMsgList
  {
  public:
    unsigned __int8 m_ucRace;
    CMsg **m_ppMsg;
    unsigned int m_uiSize;
    CNetIndexList m_kEmptyInxList;
    CNetIndexList m_kUseInxList;
    CNetIndexList m_kWaitInxList;

    bool Load(unsigned int dwCurTime);
    bool Save();
    bool SaveIndexList(unsigned int iType, CNetIndexList *kInxList);
    bool SaveMsgList(CNetIndexList *kInxList);
  };

  class CMsgListManager
  {
  public:
    bool m_bEmpty;
    CMsgList *m_pkMsgList[3];

    bool IsHaveBeenSave();
    bool Load(unsigned int dwCurTime);
    void Save();
  };
}

class __cppobj CRaceBossMsgController
{
public:
  bool m_bConnectWeb;
  int m_iOldDay;
  CMyTimer *m_pkTimer;
  RACE_BOSS_MSG::CMsgListManager m_kManager;

  static CRaceBossMsgController *Instance();
  bool Init();

  int GetCurDay();
  bool LoadCurTime(unsigned int *dwCurTime);
};
