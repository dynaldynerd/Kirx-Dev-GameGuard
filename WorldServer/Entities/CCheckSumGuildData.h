#pragma once

#include "IdaCompat.h"

class CRFWorldDatabase;

class  CCheckSumGuildData
{
public:
  CCheckSumGuildData(unsigned int dwSerial);
  ~CCheckSumGuildData();

  void Encode(long double dDalant, long double dGold);
  void Decode(long double dDalant, long double dGold);
  long double GetDalant();
  long double GetGold();
  void SetValue(unsigned int eType, long double dValue);
  bool Insert(CRFWorldDatabase *pkDB);
  bool Update(CRFWorldDatabase *pkDB);
  int Load(CRFWorldDatabase *pkDB, CCheckSumGuildData *kSrcValue);
  int CheckDiff(CRFWorldDatabase *pkDB, char *wszName, CCheckSumGuildData *kSrcValue);

  unsigned int m_dwGuildSerial;
  long double m_dValues[2];
};

