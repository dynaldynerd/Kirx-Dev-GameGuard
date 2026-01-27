#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct __cppobj _FaceItem_fld : _base_fld
{
  int m_bExist;
  char m_strModle[64];
  char m_strName[64];
  int m_nKindClt;
  int m_nFixPart;
  char m_strCivil[64];
  int m_nDefEffType;
};

#pragma pack(pop)
