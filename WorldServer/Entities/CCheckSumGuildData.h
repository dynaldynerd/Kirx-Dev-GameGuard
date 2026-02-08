#pragma once

#include "IdaCompat.h"

class CRFWorldDatabase;

class __cppobj CCheckSumGuildData
{
public:
  void Encode(long double dDalant, long double dGold);
  bool Insert(CRFWorldDatabase *pkDB);
  bool Update(CRFWorldDatabase *pkDB);

  unsigned int m_dwGuildSerial;
  long double m_dValues[2];
};

