#pragma once

#include "IdaCompat.h"
#include "StorageList.h"

class CPlayer;
struct _sf_continous;
struct _skill_fld;

/* 1763 */
class __cppobj CEquipItemSFAgent
{
public:
  struct _requireSlot
  {
    bool m_SlotIndex[8];

    _requireSlot();
    void Init();
  };

private:
  CPlayer *m_pMaster;
  _sf_continous *m_pContSF[8];

public:
  void Init(CPlayer *pMaster);
  void AllEndContSF();
  void EndContSF(_sf_continous *pSF_Cont);
  char GetRequireSFSlot(_requireSlot *pSlot, _skill_fld *pSkillFld);
  void SetSFCont(unsigned int nEquipTblIndex, _sf_continous *pSF);
  void StartContSF(_sf_continous *pSF_Cont);
  _STORAGE_LIST::_db_con *GetEquip(unsigned int nEquipTblIndex);
  _sf_continous *GetEquipSFCont(unsigned int nEquipTblIndex);
  bool IsUseBooster();
  float GetBoosterAddSpeed();
  void ReleaseSFCont(int nEquipTblIndex);
};

