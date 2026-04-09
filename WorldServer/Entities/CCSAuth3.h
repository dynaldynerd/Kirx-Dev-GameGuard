#pragma once

#include "IdaCompat.h"

class CCSAuth3
{
public:
  CCSAuth3();
  ~CCSAuth3();

  unsigned int Init(int bFirstChecker);
  unsigned int Close();
  unsigned int Get(unsigned int *puReturnedPacketSize);
  unsigned int Check(unsigned int uReturnedPacketSize);

  unsigned int uMagic;
  unsigned __int8 packet[4096];
  unsigned __int8 data[4096];
};

static_assert(sizeof(CCSAuth3) == 8196);

unsigned int InitCSAuth3(const char *pszPath);
unsigned int CloseCSAuth3();
