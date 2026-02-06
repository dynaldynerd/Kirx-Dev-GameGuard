#pragma once

#ifndef _DUMMY_POSITION_H_
#define _DUMMY_POSITION_H_

#include "IdaCompat.h"

#pragma pack(push, 1)

struct __cppobj __declspec(align(4)) _dummy_position
{
  _dummy_position()
  {
    m_bPosAble = true;
    m_wActiveMon = 0;
  }

  char m_szCode[64];
  unsigned __int16 m_wLineIndex;
  bool m_bPosAble;
  short m_zLocalMin[3];
  short m_zLocalMax[3];
  float m_fMin[3];
  float m_fMax[3];
  float m_fRT[3];
  float m_fLB[3];
  float m_fCenterPos[3];
  float m_fDirection[3];
  unsigned __int16 m_wActiveMon;
};

struct _base_fld;

struct __cppobj _store_dummy
{
  int m_nStoreType;
  _base_fld *m_pStoreRec;
  _dummy_position *m_pDumPos;
};

#pragma pack(pop)

#endif // _DUMMY_POSITION_H_
