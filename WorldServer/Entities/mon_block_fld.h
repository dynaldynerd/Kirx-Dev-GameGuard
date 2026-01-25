#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _mon_block_fld : _base_fld
{
  struct _dummy_info
  {
    char m_strDummyCode[64];
    unsigned int m_dwSelectProp;
  };

  unsigned int m_dwDummyNum;
  _dummy_info m_DummyInfo[20];
  int m_nMin;
  int m_nMob;
  int m_nMax;
};
#pragma pack(pop)
