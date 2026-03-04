#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"
#include "CMyTimer.h"

struct EventItemInfo
{
  unsigned __int8 byTableCode;
  unsigned int dwIndex;
};

class CPlayer;

class  CExchangeEvent : public RFEventBase
{
public:
  static CExchangeEvent *m_Instance;
  CExchangeEvent();
  ~CExchangeEvent();
  static CExchangeEvent *Instance();
  static void Destroy();
  bool Initialzie();
  void Loop() override;
  bool IsEnable();
  bool IsDelete();
  bool IsWait();
  EventItemInfo *GetEventItemInfo(unsigned int nInfoType);
  void DeleteExchangeEventItem(CPlayer *pOne);
  void GiveEventItem(CPlayer *pOne);

private:
  bool CheckBuddhaEventData(bool *pbDelete);
  void ChangeData();
  void ReadBuddhaEventInfo();

  bool m_bEnable;
  bool m_bDelete;
  char m_EventItemCode[4][64];
  EventItemInfo m_EventItemInfo[4];
  unsigned __int8 m_byState;
  bool m_bGiveItem;
  bool m_bWait;
  bool m_bModifyEnable;
  bool m_bModifyDelete;
  char m_ModifyItemCode[4][64];
  unsigned int m_dwPlayerArrayNo;
  CMyTimer m_tmDataFileCheckTime;
  _FILETIME m_ftWrite;
};

