#pragma once

#include "IdaCompat.h"
#include "skill_fld.h"

class CPlayer;

class  CRaceBuffInfoByHolyQuest
{
public:

  CRaceBuffInfoByHolyQuest(_skill_fld *data, unsigned __int8 lv);
  ~CRaceBuffInfoByHolyQuest();
  static CRaceBuffInfoByHolyQuest *Create(unsigned int uiNTh, char *szItemName);
  static bool LoadINISubProcLoadCode(
    unsigned int uiTh,
    char *szItemName,
    _skill_fld **ppFld,
    unsigned __int8 *byLv);
  bool Apply(CPlayer *pkDest);
  bool Release(CPlayer *pkDest);
  bool CreateComplete(CPlayer *pkDest);
  bool ApplyEffect(CPlayer *pkDest, bool bAdd);
  void NotifySetBuff(CPlayer *pkDest);
  void NotifyReleaseBuff(unsigned __int16 wUserInx);
  void NotifyLogInSetBuff(unsigned __int16 wUserInx);

  _skill_fld *m_pData;
  unsigned __int8 m_byLv;
};
