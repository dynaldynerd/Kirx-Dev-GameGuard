#include "pch.h"

#include "CDarkHole.h"

#include <cstring>

void CDarkHole::Init(_object_id *pID)
{
  CGameObject::Init(pID);
  m_dwOpenerSerial = 0;
  std::memset(m_wszOpenerName, 0, sizeof(m_wszOpenerName));
  std::memset(m_aszOpenerName, 0, sizeof(m_aszOpenerName));
}

