#pragma once

#include "IdaCompat.h"

#include "CCheckSumBaseConverter.h"

class CCheckSumGuildData;

class __cppobj CCheckSumGuildConverter : public CCheckSumBaseConverter
{
public:
  void Convert(long double dDalant, long double dGold, CCheckSumGuildData *pkCheckSum);
};

