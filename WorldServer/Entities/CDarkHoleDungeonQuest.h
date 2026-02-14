#pragma once

#include "IdaCompat.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuestSetup.h"

class CPlayer;
class CDarkHole;

class __cppobj CDarkHoleDungeonQuest : public CDarkHoleDungeonQuestSetup
{
public:
  CDarkHoleDungeonQuest();

  ~CDarkHoleDungeonQuest() override;
  bool LoadDarkHoleQuest();
  void CheckQuestOnLoop();
  CDarkHoleChannel *GetChannel(unsigned int dwChannelIndex);
  CDarkHoleChannel *CanOpenChannel(int nQuestIndex);
  int SearchEmptyDarkHoleChannel();
  int SearchEmptyDarkHoleLayer(int nQuestIndex);
  CDarkHoleChannel *OpenChannel(int nQuestIndex, CPlayer *pOpener, CDarkHole *pHoleObj);
  CDarkHoleChannel *SearchOncePlayedChannel(unsigned int dwMemberSerial);

  bool m_bLoad;
  CDarkHoleChannel m_Channel[128];
  unsigned int m_dwCheckLastTime;
  unsigned int m_dwChannelSerialCounter;
};

extern CDarkHoleDungeonQuest g_DarkHoleQuest;

