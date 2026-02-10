#pragma once

#include "IdaCompat.h"

#include "CCheckSumBaseConverter.h"

class CCheckSumCharacAccountTrunkData;
struct _AVATOR_DATA;

class __cppobj CCheckSumCharacTrunkConverter : public CCheckSumBaseConverter
{
public:
  void Convert(_AVATOR_DATA *pAvator, CCheckSumCharacAccountTrunkData *pkCheckSum);
  void ConvertTrunk(unsigned int dwSerial, long double *pVal);
};

