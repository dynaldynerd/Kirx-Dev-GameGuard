#pragma once

#include "IdaCompat.h"
#include "CTotalGuildRankInfo.h"

class __cppobj CTotalGuildRankManager
{
public:
  static CTotalGuildRankManager *Instance();
  bool Init();

  int m_iOldDay;
  CTotalGuildRankInfo m_kInfo;
};
