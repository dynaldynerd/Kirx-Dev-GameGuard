#pragma once

#include <cstring>

#include <windows.h>

constexpr unsigned __int64 kMaxLegacyLastConnTime = 4212312359ULL;

inline unsigned int ClampLegacyLastConnTime(unsigned __int64 canonicalLastConnTime)
{
  const unsigned __int64 clampedLastConnTime =
    canonicalLastConnTime > kMaxLegacyLastConnTime ? kMaxLegacyLastConnTime : canonicalLastConnTime;
  return static_cast<unsigned int>(clampedLastConnTime);
}

struct _EQUIPKEY
{
  __int16 zItemIndex;
  void SetRelease();
  bool IsFilled();
  void LoadDBKey(__int16 key);
  __int16 CovDBKey();
};

inline void _EQUIPKEY::SetRelease()
{
  zItemIndex = -1;
}

inline bool _EQUIPKEY::IsFilled()
{
  return zItemIndex != -1;
}

inline void _EQUIPKEY::LoadDBKey(__int16 key)
{
  zItemIndex = key;
}

inline __int16 _EQUIPKEY::CovDBKey()
{
  return zItemIndex;
}

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
  unsigned int m_dwLastConnTime;
  _REGED_AVATOR_DB();
  void Init();
};
#pragma pack(pop)

inline _REGED_AVATOR_DB::_REGED_AVATOR_DB()
{
  Init();
}

inline void _REGED_AVATOR_DB::Init()
{
  std::memset(this, 0, sizeof(_REGED_AVATOR_DB));
  m_bySlotIndex = static_cast<unsigned __int8>(-1);
  m_dwRecordNum = static_cast<unsigned int>(-1);
  for (int j = 0; j < 8; ++j)
  {
    m_EquipKey[j].SetRelease();
  }
}

static_assert(sizeof(_REGED_AVATOR_DB) == 69, "_REGED_AVATOR_DB packing mismatch");
