#pragma once

#include "IdaCompat.h"
#include "skill_fld.h"

class __cppobj CRaceBuffInfoByHolyQuest
{
public:

  CRaceBuffInfoByHolyQuest(_skill_fld *data, unsigned __int8 lv);
  static CRaceBuffInfoByHolyQuest *Create(unsigned int uiNTh, char *szItemName);
  static bool LoadINISubProcLoadCode(
    unsigned int uiTh,
    char *szItemName,
    _skill_fld **ppFld,
    unsigned __int8 *byLv);

  _skill_fld *m_pData;
  unsigned __int8 m_byLv;
};
