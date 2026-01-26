#include "pch.h"

#include "CMapData.h"

#include <cstring>

bool CMapData::IsMapIn(float *fPos)
{
  int scratch[4]{};
  std::memset(scratch, 0xCC, sizeof(scratch));

  for (int j = 0; j < 3; ++j)
  {
    if (fPos[j] > static_cast<float>(m_BspInfo.m_nMapMaxSize[j])
        || static_cast<float>(m_BspInfo.m_nMapMinSize[j]) > fPos[j])
    {
      return false;
    }
  }
  return true;
}

