#include "pch.h"

#include "CEquipItemSFAgent.h"

#include "CPlayer.h"

_sf_continous *CEquipItemSFAgent::GetEquipSFCont(unsigned int nEquipTblIndex)
{
  if (nEquipTblIndex >= 8)
  {
    return nullptr;
  }
  return m_pContSF[nEquipTblIndex];
}

void CEquipItemSFAgent::ReleaseSFCont(int nEquipTblIndex)
{
  if (!m_pMaster)
  {
    return;
  }

  _sf_continous *equipCont = GetEquipSFCont(static_cast<unsigned int>(nEquipTblIndex));
  if (!equipCont)
  {
    return;
  }

  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 8; ++k)
    {
      _sf_continous *cont = &m_pMaster->m_SFCont[j][k];
      if (cont->m_bExist && equipCont == cont)
      {
        m_pMaster->RemoveSFContEffect(static_cast<unsigned __int8>(j), static_cast<unsigned __int16>(k), false, false);
        return;
      }
    }
  }
}

