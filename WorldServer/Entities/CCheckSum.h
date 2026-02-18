#pragma once

#include "IdaCompat.h"

/* 1280 */
class  CCheckSum
{
public:
  CCheckSum();
  ~CCheckSum();

  char Init();
  unsigned int EncodeValue(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue);
  unsigned int DecodeValue(unsigned __int8 byIndex, unsigned int dwSerial, unsigned int dwValue);
};

