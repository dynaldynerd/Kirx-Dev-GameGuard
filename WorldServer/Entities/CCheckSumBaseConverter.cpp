#include "pch.h"

#include "CCheckSumBaseConverter.h"

namespace
{
  // NOTE: Key values are not present in the IDA function dumps. Defaulting to zeros for now.
  // These should be replaced with the real key tables when available.
  const unsigned int kCheckSumDwKeys[8][10] = {};
  const long long kCheckSumI64Keys[8][10] = {};
}

const unsigned int *CCheckSumBaseConverter::ms_dwKeys[8] = {
  kCheckSumDwKeys[0],
  kCheckSumDwKeys[1],
  kCheckSumDwKeys[2],
  kCheckSumDwKeys[3],
  kCheckSumDwKeys[4],
  kCheckSumDwKeys[5],
  kCheckSumDwKeys[6],
  kCheckSumDwKeys[7],
};

const long long *CCheckSumBaseConverter::ms_i64Keys[8] = {
  kCheckSumI64Keys[0],
  kCheckSumI64Keys[1],
  kCheckSumI64Keys[2],
  kCheckSumI64Keys[3],
  kCheckSumI64Keys[4],
  kCheckSumI64Keys[5],
  kCheckSumI64Keys[6],
  kCheckSumI64Keys[7],
};

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
