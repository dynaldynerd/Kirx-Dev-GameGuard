#include "pch.h"

#include "CCheckSum.h"

namespace
{
  // NOTE: Key values are not present in the IDA function dumps. Defaulting to zeros for now.
  // These should be replaced with the real key tables when available.
  const unsigned int kCheckSumKeys[8][10] = {};
  const unsigned int *check_key_value[8] = {
    kCheckSumKeys[0],
    kCheckSumKeys[1],
    kCheckSumKeys[2],
    kCheckSumKeys[3],
    kCheckSumKeys[4],
    kCheckSumKeys[5],
    kCheckSumKeys[6],
    kCheckSumKeys[7],
  };
}

CCheckSum::CCheckSum()
{
  // this is not a stub
}

CCheckSum::~CCheckSum()
{
  // this is not a stub
}

char CCheckSum::Init()
{
  return 1;
}

unsigned int CCheckSum::EncodeValue(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue)
{
  const unsigned int *keys = check_key_value[byIndex];
  return keys[dwSerial % 0x0A] ^ dwValue;
}

unsigned int CCheckSum::DecodeValue(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue)
{
  const unsigned int *keys = check_key_value[byIndex];
  return keys[dwSerial % 0x0A] ^ dwValue;
}
