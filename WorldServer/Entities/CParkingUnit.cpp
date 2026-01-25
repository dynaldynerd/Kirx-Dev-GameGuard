#include "pch.h"

#include "CParkingUnit.h"

void CParkingUnit::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwLastDestroyTime = 0;
  m_byFrame = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 6; ++j)
  {
    m_byPartCode[j] = static_cast<unsigned __int8>(-1);
  }
  m_byCreateType = static_cast<unsigned __int8>(-1);
  m_byTransDistCode = static_cast<unsigned __int8>(-1);
  m_wHPRate = 0;
}

