#pragma once

#include "IdaCompat.h"

struct __cppobj __declspec(align(4)) _ChatStealTargetInfo
{
  unsigned __int8 m_byStealType;
  unsigned int m_dwTargetSerial;
  unsigned __int8 m_byRaceBoss;

  _ChatStealTargetInfo();
};

inline _ChatStealTargetInfo::_ChatStealTargetInfo()
  : m_byStealType(0), m_dwTargetSerial(static_cast<unsigned int>(-1)), m_byRaceBoss(static_cast<unsigned __int8>(-1))
{
}
