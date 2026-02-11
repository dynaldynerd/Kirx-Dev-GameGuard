#pragma once

#include "IdaCompat.h"

#include "ObjectCreateSetData.h"

class CPlayer;

struct __cppobj _nuclear_create_setdata : _character_create_setdata
{
  CPlayer *pMaster;
  int nMasterSirial;
  unsigned int m_dwWarnTime;
  unsigned int m_dwAttInformTime;
  unsigned int m_dwAttStartTime;

  _nuclear_create_setdata()
  {
    pMaster = nullptr;
    nMasterSirial = 0;
    m_dwWarnTime = 0;
    m_dwAttInformTime = 0;
    m_dwAttStartTime = 0;
  }
};
