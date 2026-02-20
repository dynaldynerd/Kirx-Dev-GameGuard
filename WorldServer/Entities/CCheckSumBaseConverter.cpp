#include "pch.h"

#include "CCheckSumBaseConverter.h"

const unsigned int *CCheckSumBaseConverter::ms_dwKeys[8];

const long long *CCheckSumBaseConverter::ms_i64Keys[8];

unsigned int CCheckSumBaseConverter::ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue)
{
  const unsigned int *keys = ms_dwKeys[byIndex];
  return keys[dwSerial % 0x0A] ^ dwValue;
}

long double CCheckSumBaseConverter::ProcCode(unsigned __int8 byIndex, unsigned int dwSerial, long double dValue)
{
  const long long *keys = ms_i64Keys[byIndex];
  const unsigned int keyValue = static_cast<unsigned int>(keys[dwSerial % 0x0A]);
  return static_cast<long double>(keyValue ^ static_cast<unsigned int>(static_cast<int>(dValue)));
}
