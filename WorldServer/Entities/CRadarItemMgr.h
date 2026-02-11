#pragma once

#include "IdaCompat.h"

class CPlayer;
class CMapData;
struct _RadarItem_fld;

/* 1596 */
class __cppobj __declspec(align(8)) CRadarItemMgr
{
public:
  bool m_bUse;
  bool m_bUpdate;
  unsigned int m_dwStartTime;
  unsigned int m_dwDurTime;
  unsigned int m_dwDelayTime;
  CPlayer *m_pMaster;
  char m_strRadarCode[64];
  CMapData *m_pDestMap;
  bool m_bPlayerEnd;
  bool m_bMonEnd;
  int m_nPlayerNum;
  int m_nMonNum;
  bool m_bSameRace;
  bool m_bNorDiffRace;
  bool m_bChiefDiffRace;
  bool m_bEliteMonster;
  _detected_char_list m_RadarResult;

  void Init();
  void Init(unsigned int dwDelayTime);
  bool IsUse();
  void SetUseRadar(char (*strRadarCode)[64], CPlayer *pMaster, unsigned int dwDurTime, unsigned int dwDelayTime);
  char RadarProc(_RadarItem_fld *pRadarFld);
  void ResetFlags();
  void ResetUpdate();
  unsigned int CalcDelay();
  unsigned int GetDelayTime();
};

