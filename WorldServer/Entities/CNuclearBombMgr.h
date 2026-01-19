#pragma once

#include "IdaCompat.h"

#include "CNuclearBomb.h"

class __cppobj __declspec(align(8)) CNuclearBombMgr
{
public:
  virtual ~CNuclearBombMgr() = default;

  char m_szStickCode[64];
  CNuclearBomb m_Missile[3][3];
  unsigned int m_dwWarnTime;
  unsigned int m_dwAttInformTime;
  unsigned int m_dwAttStartTime;
};
