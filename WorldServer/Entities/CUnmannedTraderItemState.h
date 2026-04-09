#pragma once

#include "IdaCompat.h"

class  CUnmannedTraderItemState
{
public:
  enum class STATE : int
  {
    FIREEL = 0,
    WATEREL = 1,
    SOILEL = 2,
    WINDEL = 3,
    NONE = -1,
  };

  CUnmannedTraderItemState();
  ~CUnmannedTraderItemState();
  void Clear();
  bool Set(unsigned __int8 byState);
  unsigned int GetState() const;

  static unsigned int GetMaxStateCnt();
  static wchar_t **GetStateStrList();
  static wchar_t *GetStateStrW(unsigned int uiInx);
  static bool PushUpdateState(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 byState,
    unsigned int dwOwnerSerial,
    unsigned __int16 wItemSerial,
    unsigned __int8 byItemTableCode,
    unsigned __int16 wItemTableIndex);

  STATE m_eState;

  static wchar_t *ms_wszStateName[16];
};
