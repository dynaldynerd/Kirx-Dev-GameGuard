#pragma once

#include "IdaCompat.h"

class CPlayer;
struct _sf_continous;

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

public:
  _sf_continous *GetEquipSFCont(unsigned int nEquipTblIndex);
  void ReleaseSFCont(int nEquipTblIndex);
};

