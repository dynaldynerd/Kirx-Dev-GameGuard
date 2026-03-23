#pragma once

#include "IdaCompat.h"

struct _EQUIPKEY
{
  __int16 zItemIndex;
  void SetRelease();
  bool IsFilled();
  void LoadDBKey(__int16 key);
  __int16 CovDBKey();
};

#pragma pack(push, 1)
struct _REGED_AVATOR_DB
{
  char m_wszAvatorName[17];
  unsigned int m_dwRecordNum;
  unsigned __int8 m_byRaceSexCode;
  unsigned __int8 m_bySlotIndex;
  char m_szClassCode[5];
  unsigned __int8 m_byLevel;
  unsigned int m_dwDalant;
  unsigned int m_dwGold;
  unsigned int m_dwBaseShape;
  _EQUIPKEY m_EquipKey[8];
  unsigned __int8 m_byEquipLv[8];
  unsigned __int64 m_dwLastConnTime;
  _REGED_AVATOR_DB();
  void Init();
};
#pragma pack(pop)
