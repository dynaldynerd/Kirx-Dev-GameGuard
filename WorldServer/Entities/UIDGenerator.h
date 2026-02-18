#pragma once

#include "IdaCompat.h"
#include "CNetCriticalSection.h"

class  UIDGenerator
{
public:
  static unsigned __int64 getuid(unsigned __int8 n);
  static unsigned __int64 getuid(unsigned __int8 n, unsigned __int8 ncode);
  static void tmuid(unsigned __int64 uid, char *szBuf);

  static CNetCriticalSection _cs;
};
