#include "CCharacterDataMgr.h"

#include "ClientDataPath.h"

CCharacterDataMgr *g_pCharDataMgr = NULL;

namespace
{
BOOL LoadNationFile(CEdfFile &po_rFile, const char *pi_pFileName)
{
  char l_szFileName[MAX_PATH];
  if (!GetNationDataFileName(pi_pFileName, l_szFileName, sizeof(l_szFileName)))
  {
    return FALSE;
  }

  return po_rFile.Load(l_szFileName);
}

BOOL IsExistingFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES &&
         (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

BOOL LoadOptionalFile(CEdfFile &po_rFile, const char *pi_pFileName)
{
  if (!IsExistingFile(pi_pFileName))
  {
    return TRUE;
  }

  return po_rFile.Load(pi_pFileName);
}

BOOL LoadOptionalNationFile(CEdfFile &po_rFile, const char *pi_pFileName)
{
  char l_szFileName[MAX_PATH];
  if (!GetNationDataFileName(pi_pFileName, l_szFileName, sizeof(l_szFileName)))
  {
    return TRUE;
  }

  return po_rFile.Load(l_szFileName);
}
} // namespace

CCharacterDataMgr::CCharacterDataMgr()
  : m_bCreated(FALSE),
    m_bLoaded(FALSE)
{
}

CCharacterDataMgr::~CCharacterDataMgr()
{
  Destroy();
}

BOOL CCharacterDataMgr::Create(void)
{
  if (m_bCreated)
  {
    return TRUE;
  }

  m_bCreated = TRUE;
  return TRUE;
}

BOOL CCharacterDataMgr::Destroy(void)
{
  UnloadData();
  m_bCreated = FALSE;
  return TRUE;
}

BOOL CCharacterDataMgr::LoadData(void)
{
  if (!m_bCreated)
  {
    return FALSE;
  }

  UnloadData();

  if (!m_cCharacterData.Load(".\\DataTable\\character.edf") ||
      !m_cPlayerCharacterData.Load(".\\DataTable\\PlayerCharacter.edf") ||
      !m_cSkillForceData.Load(".\\DataTable\\SkillForce.edf") ||
      !m_cStoreData.Load(".\\DataTable\\Store.edf") ||
      !m_cQuestData.Load(".\\DataTable\\Quest.edf") ||
      !m_cMsgEventData.Load(".\\DataTable\\MsgEvent.edf") ||
      !m_cEventShipData.Load(".\\DataTable\\EventShip.edf") ||
      !LoadNationFile(m_cNationCharacterData, "NDCharacter.edf") ||
      !LoadNationFile(m_cNationSkillForceData, "NDSkillForce.edf") ||
      !LoadNationFile(m_cNationStoreData, "NDStore.edf") ||
      !LoadNationFile(m_cNationQuestData, "NDQuest.edf") ||
      !LoadNationFile(m_cNationMsgEventData, "NDMsgEvent.edf") ||
      !LoadNationFile(m_cNationEventShipData, "NDEventShip.edf") ||
      !LoadOptionalNationFile(m_cNationLanguageData, "NDLanguage.edf") ||
      !LoadOptionalNationFile(m_cNationMsgMonsterData, "NDMsgMonster.edf") ||
      !LoadOptionalNationFile(m_cNationUIHelpData, "UIHelp.edf") ||
      !LoadOptionalNationFile(m_cNationPlayerData, "Player.edf") ||
      !LoadOptionalNationFile(m_cNationExpData, "Exp.edf") ||
      !LoadOptionalNationFile(m_cNationGameData, "GameData.edf") ||
      !LoadOptionalNationFile(m_cNationHintData, "Hint.edf") ||
      !LoadOptionalNationFile(m_cNationPcRoomData, "PcRoom.edf") ||
      !LoadOptionalFile(m_cEventSoundData, ".\\DataTable\\EventSound.edf") ||
      !LoadOptionalFile(m_cSpecialStoreData, ".\\DataTable\\SpecialStore.edf"))
  {
    UnloadData();
    return FALSE;
  }

  m_bLoaded = TRUE;
  return TRUE;
}

void CCharacterDataMgr::UnloadData(void)
{
  m_cCharacterData.Unload();
  m_cPlayerCharacterData.Unload();
  m_cSkillForceData.Unload();
  m_cStoreData.Unload();
  m_cQuestData.Unload();
  m_cMsgEventData.Unload();
  m_cEventShipData.Unload();

  m_cNationCharacterData.Unload();
  m_cNationSkillForceData.Unload();
  m_cNationStoreData.Unload();
  m_cNationQuestData.Unload();
  m_cNationMsgEventData.Unload();
  m_cNationEventShipData.Unload();
  m_cNationLanguageData.Unload();
  m_cNationMsgMonsterData.Unload();
  m_cNationUIHelpData.Unload();
  m_cNationPlayerData.Unload();
  m_cNationExpData.Unload();
  m_cNationGameData.Unload();
  m_cNationHintData.Unload();
  m_cNationPcRoomData.Unload();

  m_cEventSoundData.Unload();
  m_cSpecialStoreData.Unload();

  m_bLoaded = FALSE;
}

BOOL CCharacterDataMgr::IsLoaded(void) const
{
  return m_bLoaded;
}
