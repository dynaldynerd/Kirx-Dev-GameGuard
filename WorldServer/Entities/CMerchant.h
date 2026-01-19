#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

class CItemStore;

class __cppobj CMerchant : public CCharacter
{
public:
  CItemStore *m_pItemStore;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwLastDestroyTime;
  int m_nLeftTicketNum[2];
};
