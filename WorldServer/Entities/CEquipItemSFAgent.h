#pragma once

#include "IdaCompat.h"

/* 1763 */
class __cppobj CEquipItemSFAgent
{
public:
  struct _requireSlot
  {
    bool m_SlotIndex[8];
  };

private:
  CPlayer *m_pMaster;
  _sf_continous *m_pContSF[8];
};

