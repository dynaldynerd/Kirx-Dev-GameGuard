#pragma once

#include "CMainThread.h"

struct _event_config_classrefine
{
  bool bEnable;
  int nMaxRefineCnt;
  unsigned int nStartDate;
  unsigned int nEndDate;
};

struct __declspec(align(4)) _event_participant_classrefine
{
  unsigned __int16 nSock;
  unsigned int nAvatorSerial;
  bool bChange;
  unsigned int dwRefineDate;
  unsigned __int8 nCurRefineCnt;
};

#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME
{
  unsigned int dwLowDateTime;
  unsigned int dwHighDateTime;
} FILETIME;
#endif

class __cppobj RFEvent_ClassRefine : public RFEventBase
{
public:
  bool Initialzie();

  void ReadClassRefineEventInfo();

  _event_config_classrefine _kEvent;
  _event_participant_classrefine *_pkParticipant;
  bool m_bUserDataReset;
  bool m_bDateReset;
  _event_config_classrefine _kModifyEvent;
  CMyTimer m_tmDataFileCheckTime;
  _FILETIME m_ftWrite;
};
