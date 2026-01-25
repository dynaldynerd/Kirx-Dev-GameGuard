#pragma once

#include "IdaCompat.h"
#include "base_fld.h"
#include "SkillCommon.h"

#pragma pack(push, 1)
struct __cppobj _portal_fld : _base_fld
{
  char m_strLinkMapCode[64];
  char m_strLinkPortalCode[64];
  char m_szMenu[64];
  int m_bNeedConCheck;
  int m_nNeedChrLevel;
  int m_nUpLevelLim;
  _consume_item_list m_ConsumeItemList[3];
  int m_nKind;
  char m_strUseRace[64];
};
#pragma pack(pop)
