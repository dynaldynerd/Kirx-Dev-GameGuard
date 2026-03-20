#pragma once

#include "CMainThread.h"

struct _event_config_classrefine
{
  bool bEnable;
  int nMaxRefineCnt;
  unsigned int nStartDate;
  unsigned int nEndDate;
};

struct _event_participant_classrefine
{
  unsigned __int16 nSock;
  unsigned int nAvatorSerial;
  bool bChange;
  unsigned int dwRefineDate;
  unsigned __int8 nCurRefineCnt;

  bool IsChanged();
  int size();
};

#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME
{
  unsigned int dwLowDateTime;
  unsigned int dwHighDateTime;
} FILETIME;
#endif

class  RFEvent_ClassRefine : public RFEventBase
{
public:
  RFEvent_ClassRefine();
  bool Initialzie();
  bool IsEnable();
  void Loop();
  bool CheckRefineEventData();
  void ResetRefineData();
  int CanDoEvent(CPlayer *pOne);
  int DoEvent(CPlayer *pOne) override;
  bool SetEvent(const char *p, int size, bool bInit) override;
  bool IsDbUpdate(unsigned int nIdx) override;
  _event_participant_classrefine *GetPlayerState(unsigned int nIdx, unsigned int nAvator) override;
  bool SetPlayerState(void *const p, int size) override;

  void ReadClassRefineEventInfo();

  _event_config_classrefine _kEvent;
  _event_participant_classrefine *_pkParticipant;
  bool m_bUserDataReset;
  bool m_bDateReset;
  _event_config_classrefine _kModifyEvent;
  CMyTimer m_tmDataFileCheckTime;
  _FILETIME m_ftWrite;
};

