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
};

extern CCharacterDataMgr *g_pCharDataMgr;

#define _GetCharDataMgr() g_pCharDataMgr
