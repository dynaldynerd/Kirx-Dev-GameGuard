#pragma once

#include <windows.h>

#include "EdfFile.h"

class CCharacterDataMgr
{
public:
  CCharacterDataMgr();
  ~CCharacterDataMgr();

  BOOL Create(void);
  BOOL Destroy(void);

  BOOL LoadData(void);
  void UnloadData(void);
  BOOL IsLoaded(void) const;

private:
  BOOL m_bCreated;
  BOOL m_bLoaded;

  CEdfFile m_cCharacterData;
  CEdfFile m_cPlayerCharacterData;
  CEdfFile m_cSkillForceData;
  CEdfFile m_cStoreData;
  CEdfFile m_cQuestData;
  CEdfFile m_cMsgEventData;
  CEdfFile m_cEventShipData;

  CEdfFile m_cNationCharacterData;
  CEdfFile m_cNationSkillForceData;
  CEdfFile m_cNationStoreData;
  CEdfFile m_cNationQuestData;
  CEdfFile m_cNationMsgEventData;
  CEdfFile m_cNationEventShipData;
  CEdfFile m_cNationLanguageData;
  CEdfFile m_cNationMsgMonsterData;
  CEdfFile m_cNationUIHelpData;
  CEdfFile m_cNationPlayerData;
  CEdfFile m_cNationExpData;
  CEdfFile m_cNationGameData;
  CEdfFile m_cNationHintData;
  CEdfFile m_cNationPcRoomData;

  CEdfFile m_cEventSoundData;
  CEdfFile m_cSpecialStoreData;
};

extern CCharacterDataMgr *g_pCharDataMgr;

#define _GetCharDataMgr() g_pCharDataMgr
