#pragma once

#include "IdaCompat.h"

struct  _action_point_system_ini
{
  bool m_bUse_event;
  bool m_bReset;
  int m_wYear[2];
  int m_byMonth[2];
  int m_byDay[2];
  int m_byHour[2];
  int m_byMinute[2];
  int m_EventTime[2];
  unsigned __int8 m_byActionCode;

  _action_point_system_ini();
};

class  CActionPointSystemMgr
{
public:
  CActionPointSystemMgr();
  ~CActionPointSystemMgr();

  static CActionPointSystemMgr *Instance();
  bool Initialize();

  void Load_Event_INI(_action_point_system_ini *pIni, unsigned __int16 wIndex);
  void Check_Load_Event_Status(unsigned __int8 byActionCode, _action_point_system_ini *pIni);
  void Check_Event_Status();
  void Check_Loop();
  void SetEventStatus(unsigned __int8 byActionCode, unsigned __int8 byStatus);
  unsigned __int8 GetEventStatus(unsigned __int8 byActionCode);
  bool IsSystemEnable(unsigned __int8 byActionCode);
  bool IsPointReset(unsigned __int8 byActionCode);

  _action_point_system_ini m_st_ini_list[3];
  unsigned __int8 m_bActive[3];
};


