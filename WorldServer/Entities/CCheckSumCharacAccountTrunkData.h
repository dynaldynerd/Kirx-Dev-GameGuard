#pragma once

#include "IdaCompat.h"

class CRFWorldDatabase;
struct _AVATOR_DATA;

class  CCheckSumCharacAccountTrunkData
{
public:
  CCheckSumCharacAccountTrunkData(unsigned int dwSerial, unsigned int dwAccountSerial, unsigned __int8 byRace);
  ~CCheckSumCharacAccountTrunkData();

  void Encode(_AVATOR_DATA *pAvator);
  void Decode(_AVATOR_DATA *pAvator);
  void SetValue(unsigned int eType, unsigned int dwValue);
  void SetValue(unsigned int eType, long double dValue);
  bool InsertCharacData(CRFWorldDatabase *pkDB);
  bool InsertTrunkData(CRFWorldDatabase *pkDB);
  bool Update(CRFWorldDatabase *pkDB);
  int Load(CRFWorldDatabase *pkDB, CCheckSumCharacAccountTrunkData *kSrcValue);
  int CheckDiff(CRFWorldDatabase *pkDB, char *wszName, CCheckSumCharacAccountTrunkData *kSrcValue);

  unsigned int m_dwSerial;
  unsigned int m_dwAccountSerial;
  unsigned __int8 m_byRace;
  unsigned int m_dwValues[6];
  long double m_dValues[2];
};

