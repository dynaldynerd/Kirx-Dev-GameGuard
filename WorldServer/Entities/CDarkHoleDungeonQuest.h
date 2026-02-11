#pragma once

#include "IdaCompat.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuestSetup.h"

class __cppobj CDarkHoleDungeonQuest : public CDarkHoleDungeonQuestSetup
{
public:
  bool LoadDarkHoleQuest();
  CDarkHoleChannel *SearchOncePlayedChannel(unsigned int dwMemberSerial);

  bool m_bLoad;
  CDarkHoleChannel m_Channel[128];
  unsigned int m_dwCheckLastTime;
  unsigned int m_dwChannelSerialCounter;
};

extern CDarkHoleDungeonQuest g_DarkHoleQuest;

