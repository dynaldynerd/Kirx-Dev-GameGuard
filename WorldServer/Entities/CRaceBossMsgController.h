#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

namespace RACE_BOSS_MSG
{
  class CMsg
  {
  public:
    unsigned int m_uiState;
    unsigned int m_dwID;
    unsigned int m_dwSerial;
    char m_wszName[17];
    char m_wszMsg[49];
    unsigned int m_dwSendTime;
    unsigned int m_dwWebSendDBID;

    CMsg(unsigned __int8 ucRace, unsigned int dwID);
    void Clear();
    char Set(unsigned int dwSerial, const char *pwszName, const char *pwszMsg, unsigned int dbWebSendDBID);
    unsigned int GetID();
    unsigned int GetSerial();
    char *GetBossName();
    char *GetMsg();
    unsigned int GetWebDBID();
    void SetDayChanged();
    void SetDone();
    bool IsWait();
    bool IsDayChanged();
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

    CMsgList(unsigned __int8 ucRace, unsigned int uiSize);
    ~CMsgList();
    char Init();
    void CleanUp();
    CMsg *GetEmpty();
    void AddUse(CMsg *pkMsg);
    void RollBack();
    int Cancel(unsigned int dwMsgID, CMsg **pkMsg);
    void AddEmpty(CMsg *pkMsg);
    void Release(CMsg *pkMsg);
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

    CMsgListManager();
    ~CMsgListManager();
    char Init();
    void CleanUp();
    int Send(
      unsigned __int8 ucRace,
      unsigned int dwSerial,
      const char *pwszName,
      const char *pwszMsg,
      CMsg **pkSend,
      unsigned int dbWebSendDBID);
    int Cancel(unsigned __int8 ucRace, unsigned int dwMsgID, CMsg **pkMsg);
    void CleanUpCancel(unsigned __int8 ucRace, CMsg *pkMsg);
    bool IsHaveBeenSave();
    bool Load(unsigned int dwCurTime);
    void Save();
  };
}

class CPlayer;

class __cppobj CRaceBossMsgController
{
public:
  bool m_bConnectWeb;
  int m_iOldDay;
  CMyTimer *m_pkTimer;
  RACE_BOSS_MSG::CMsgListManager m_kManager;

  static CRaceBossMsgController *Instance();
  bool Init();

  char Send(CPlayer *pkSender, const char *pwszMsg);
  char Send(
    unsigned __int8 ucRace,
    unsigned int dwSerial,
    const char *wszName,
    const char *pwszMsg,
    unsigned int dwWebSendDBID);
  void SendMsgRequestResult(unsigned __int16 usInx, char ucRet);
  void SendWebRaceBossSMSErrorResult(char iRet, unsigned int dwWebDBID);
  void SendComfirmWeb(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg);
  void SendConfirmCtrl(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg);
  char Cancel(unsigned __int8 ucRace, unsigned int dwMsgID);
  char Cancel(unsigned __int8 ucRace, unsigned int dwMsgID, CPlayer *pkManager);
  void SendCancleInfomSender(unsigned int dwSerial);
  void SendCancleInfomManager(
    unsigned __int16 usInx,
    char ucRet,
    unsigned int dwMsgID,
    const char *pwszName);
  void SendCancelWeb(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg);

  int GetCurDay();
  bool LoadCurTime(unsigned int *dwCurTime);
};
