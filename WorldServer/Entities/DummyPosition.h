#pragma once

#ifndef _DUMMY_POSITION_H_
#define _DUMMY_POSITION_H_

#include "IdaCompat.h"

#pragma pack(push, 4)

struct  _dummy_position
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

  void SetActiveMonNum(__int16 nAlter);
};

#pragma pack(pop)

struct _base_fld;

#pragma pack(push, 8)
struct  _store_dummy
{
  int m_nStoreType;
  _base_fld *m_pStoreRec;
  _dummy_position *m_pDumPos;
};
#pragma pack(pop)

static_assert(sizeof(_dummy_position) == 156, "_dummy_position size must match IDA (156)");
static_assert(alignof(_dummy_position) == 4, "_dummy_position alignment must match IDA (4)");
static_assert(sizeof(_store_dummy) == 24, "_store_dummy size must match IDA (24)");

#endif // _DUMMY_POSITION_H_

