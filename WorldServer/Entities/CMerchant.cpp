#include "pch.h"

#include "CMerchant.h"

#include <cstring>

void CMerchant::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pItemStore = nullptr;
  m_dwLastDestroyTime = 0;
  m_byRaceCode = 0;
  std::memset(m_nLeftTicketNum, -1, sizeof(m_nLeftTicketNum));
}
