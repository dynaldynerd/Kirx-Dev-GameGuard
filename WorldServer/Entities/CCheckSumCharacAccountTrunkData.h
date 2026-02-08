#pragma once

#include "IdaCompat.h"

class CRFWorldDatabase;
struct _AVATOR_DATA;

class __cppobj CCheckSumCharacAccountTrunkData
{
public:
  void Encode(_AVATOR_DATA *pAvator);
  bool Update(CRFWorldDatabase *pkDB);

  unsigned int m_dwSerial;
  unsigned int m_dwAccountSerial;
  unsigned __int8 m_byRace;
  unsigned int m_dwValues[6];
  long double m_dValues[2];
};

