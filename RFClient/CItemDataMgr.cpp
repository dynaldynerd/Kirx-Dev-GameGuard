#include "CItemDataMgr.h"

#include "ClientDataPath.h"

CItemDataMgr *g_pItemDataMgr = NULL;

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
} // namespace

CItemDataMgr::CItemDataMgr()
  : m_bCreated(FALSE),
    m_bLoaded(FALSE)
{
}

CItemDataMgr::~CItemDataMgr()
{
  Destroy();
}

BOOL CItemDataMgr::Create(void)
{
  if (m_bCreated)
  {
    return TRUE;
  }

  m_bCreated = TRUE;
  return TRUE;
}

BOOL CItemDataMgr::Destroy(void)
{
  UnloadData();
  m_bCreated = FALSE;
  return TRUE;
}

BOOL CItemDataMgr::LoadData(void)
{
  if (!m_bCreated)
  {
    return FALSE;
  }

  UnloadData();

  if (!m_cItemData.Load(".\\DataTable\\item.edf") ||
      !m_cItemCashData.Load(".\\DataTable\\ItemCash.edf") ||
      !m_cItemCombineData.Load(".\\DataTable\\ItemCombine.edf") ||
      !m_cItemPremiumData.Load(".\\DataTable\\ItemPremium.edf") ||
      !m_cItemSetEffectData.Load(".\\DataTable\\ItemSetEffect.edf") ||
      !m_cItemTimerData.Load(".\\DataTable\\ItemTimer.edf") ||
      !LoadNationFile(m_cNationItemData, "NDItem.edf") ||
      !LoadNationFile(m_cNationItemCashData, "NDItemCash.edf"))
  {
    UnloadData();
    return FALSE;
  }

  m_bLoaded = TRUE;
  return TRUE;
}

void CItemDataMgr::UnloadData(void)
{
  m_cItemData.Unload();
  m_cItemCashData.Unload();
  m_cItemCombineData.Unload();
  m_cItemPremiumData.Unload();
  m_cItemSetEffectData.Unload();
  m_cItemTimerData.Unload();
  m_cNationItemData.Unload();
  m_cNationItemCashData.Unload();
  m_bLoaded = FALSE;
}

BOOL CItemDataMgr::IsLoaded(void) const
{
  return m_bLoaded;
}
