#include "pch.h"

#include "CMainThread.h"

_dh_reward_sub_setup::_dh_reward_sub_setup()
{
  for (int i = 0; i < 4; ++i)

  nItemNum = 0;
  for (int j = 0; j < 128; ++j)
  {
    Item[j] = nullptr;
    m_dwGivePercent[j] = 0;
  }
  dExp = 0.0;
  dwPvp = 0;
  dwDalant = 0;
}

_dh_reward_sub_setup::~_dh_reward_sub_setup()
{

  for (int j = 0; j < 128; ++j)
  {
    if (Item[j])
    {
      operator delete(Item[j]);
      Item[j] = nullptr;
    }
  }
}

__dp_mission_potal::__dp_mission_potal()
{

  memset_0(this, 0, sizeof(*this));
}

__dp_mission_potal::~__dp_mission_potal() = default;

void __dp_mission_potal::set(char *szCode)
{

  m_nCodeLen[m_nNums] = static_cast<int>(strlen_0(szCode));
  strcpy_0(m_strCodes[static_cast<__int64>(m_nNums)], szCode);
  ++m_nNums;
}

bool __dp_mission_potal::find(char *pcode)
{

  for (int j = 0; j < m_nNums; ++j)
  {
    if (!strncmp(pcode, m_strCodes[static_cast<__int64>(j)], m_nCodeLen[j]))
      return true;
  }
  return false;
}

_dh_quest_setup::_dh_quest_setup()
{

  nEmbMissionSetupNum = 0;
  for (int j = 0; j < 100; ++j)
    EmbMissionSetup[j] = nullptr;
  bRealBoss = 1;
  bPartyOnly = 0;
  dwMaxMember = 8;
  pUseMap = nullptr;
  dwMonRepIndex = 0;
  pStartMissionSetup = nullptr;
  nDummyBlockNum = 0;
  nMonsterGroupNum = 0;
  sprintf(szQuestTitle, "NO TITLE");
  sprintf(szDescirptCode, "NO DATA");
}

_dh_quest_setup::~_dh_quest_setup()
{

  for (int j = 0; j < 100; ++j)
  {
    if (EmbMissionSetup[j])
    {
      delete EmbMissionSetup[j];
      EmbMissionSetup[j] = nullptr;
    }
  }
}

void _dh_quest_setup::SetRealBoss(bool isReal)
{
  bRealBoss = isReal;
}

_dh_mission_setup *_dh_quest_setup::SearchMissionFromTitle(char *pszMissionTitle)
{

  for (int j = 0; j < nEmbMissionSetupNum; ++j)
  {
    if (!strcmp_0(EmbMissionSetup[j]->szMissionTitle, pszMissionTitle))
      return EmbMissionSetup[j];
  }
  return nullptr;
}

_dh_mission_setup::_dh_mission_setup()
{

  nEmbJobSetupNum = 0;
  for (int j = 0; j < 8; ++j)
    EmbJobSetup[j] = nullptr;
  pAreaDummy = nullptr;
  pStartDummy = nullptr;
  byJobOrder = 0;
  dwLimTimeMSec = 0;
  byResultType = static_cast<unsigned __int8>(-1);
  pNextMission = nullptr;
  pszNextMissionTitle = nullptr;
  sprintf(szMissionTitle, "NO TITLE");
  sprintf(szDescirptCode, "NO DATA");
  sprintf(szCompleteMsg, "NO DATA");
  nLootItemNum = 0;
  for (int j = 0; j < 32; ++j)
    pLootItem[j] = nullptr;
  nAddMonsterNum = 0;
  for (int j = 0; j < 32; ++j)
    pAddMonster[j] = nullptr;
  nChangeMonsterNum = 0;
  for (int j = 0; j < 32; ++j)
    pChangeMonster[j] = nullptr;
  nInnerCheckNum = 0;
  for (int j = 0; j < 64; ++j)
    pInnerCheck[j] = nullptr;
  nRespondCheckNum = 0;
  for (int j = 0; j < 32; ++j)
    pRespondCheck[j] = nullptr;
  nRespawnMonsterNum = 0;
  for (int j = 0; j < 32; ++j)
    pRespawnMonster[j] = nullptr;
  nAddSecCheckNum = 0;
  for (int j = 0; j < 32; ++j)
    pAddSecCheck[j] = nullptr;
}

_dh_mission_setup::~_dh_mission_setup()
{

  for (int j = 0; j < 8; ++j)
  {
    if (EmbJobSetup[j])
      delete EmbJobSetup[j];
    EmbJobSetup[j] = nullptr;
  }
  if (pszNextMissionTitle)
  {
    operator delete[](pszNextMissionTitle);
    pszNextMissionTitle = nullptr;
  }
  for (int j = 0; j < 32; ++j)
  {
    if (pAddMonster[j])
      operator delete(pAddMonster[j]);
    pAddMonster[j] = nullptr;
  }
  for (int j = 0; j < 32; ++j)
  {
    if (pLootItem[j])
      operator delete(pLootItem[j]);
    pLootItem[j] = nullptr;
  }
  for (int j = 0; j < 32; ++j)
  {
    if (pChangeMonster[j])
      delete pChangeMonster[j];
    pChangeMonster[j] = nullptr;
  }
  for (int j = 0; j < 64; ++j)
  {
    if (pInnerCheck[j])
      delete pInnerCheck[j];
    pInnerCheck[j] = nullptr;
  }
  for (int j = 0; j < 32; ++j)
  {
    if (pRespondCheck[j])
      delete pRespondCheck[j];
    pRespondCheck[j] = nullptr;
  }
}

_react_obj::_react_obj()
{

  memset_0(this, 0, sizeof(*this));
}

void _react_obj::copy(_react_obj *pObj)
{

  memcpy_0(this, pObj, sizeof(*this));
}

_react_area::_react_area()
{

  memset_0(this, 0, sizeof(*this));
}

void _react_area::copy(_react_area *pObj)
{

  memcpy_0(this, pObj, sizeof(*this));
}

__respawn_monster::__respawn_monster()
{

  bCallEvent = false;
  pszDefineCode = nullptr;
}

__dummy_block::__dummy_block()
{
  pszBlockName = nullptr;
  nSubDummyNum = 0;
}

__monster_group::__monster_group()
{
  pszGroupName = nullptr;
  nSubMonsterNum = 0;
}

__add_monster::__add_monster()
{
}

__add_loot_item::__add_loot_item()
{

  byItemTableCode = static_cast<unsigned __int8>(-1);
}

__change_monster::__change_monster()
{
  pszIfMissionDescirptCode = nullptr;
  pszifCompleteMsg = nullptr;
}

__change_monster::~__change_monster()
{

  if (pszIfMissionDescirptCode)
    operator delete[](pszIfMissionDescirptCode);
  if (pszifCompleteMsg)
    operator delete[](pszifCompleteMsg);
}

__inner_check::__inner_check()
{

  pszMsg = nullptr;
  pszRespawnCode = nullptr;
}

__inner_check::~__inner_check()
{

  if (pszMsg)
    operator delete[](pszMsg);
  if (pszRespawnCode)
    operator delete[](pszRespawnCode);
}

__respond_check::__respond_check()
{

  pszMsg = nullptr;
}

__respond_check::~__respond_check()
{

  if (pszMsg)
    operator delete[](pszMsg);
}

__add_time::__add_time()
{

  pszMsg = nullptr;
}

_job_sub_setup::_job_sub_setup()
{
  pEventFld = nullptr;
  byTable = static_cast<unsigned __int8>(-1);
  nEventCount = 0;
}

_dh_job_setup::_dh_job_setup()
{

  nReactNum = 0;
  for (int j = 0; j < 10; ++j)
    ReactSetup[j] = nullptr;
  eventType = dh_event_take;
  sprintf(szJobTitle, "NO TITLE");
  sprintf(szDescirptCode, "NO DATA");
}

_dh_job_setup::~_dh_job_setup()
{

  for (int j = 0; j < 10; ++j)
  {
    if (ReactSetup[j])
    {
      operator delete(ReactSetup[j]);
      ReactSetup[j] = nullptr;
    }
  }
}

_react_sub_setup::_react_sub_setup()
{
  byReactType = 0;
  pPortalDummy = nullptr;
}
