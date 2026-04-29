#include "SpriteMgr.h"

#include <cstdio>
#include <cstring>

#include "ClientDataPath.h"

namespace
{
BOOL IsExistingFile(const char *pi_pFileName)
{
  if (!pi_pFileName || !pi_pFileName[0])
  {
    return FALSE;
  }

  const DWORD l_dwAttributes = GetFileAttributesA(pi_pFileName);
  return l_dwAttributes != INVALID_FILE_ATTRIBUTES && (l_dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

const char *GetSpriteBaseFileName(WORD pi_wSpriteID)
{
  switch (pi_wSpriteID)
  {
    case SP_ID_MOUSE:
      return "mouse.spr";

    case SP_ID_COMMON:
      return "common.spr";

    case SP_ID_GAUGE_EXP:
    case SP_ID_BELT:
    case SP_ID_CHAT:
      return "main.spr";

    case SP_ID_INVENTORY:
      return "inventory.spr";

    case SP_ID_SHOP:
      return "shop.spr";

    case SP_ID_LOGIN:
      return "login.spr";

    case SP_ID_LOADING:
      return "loading.spr";

    case SP_ID_ITEM:
      return "item.spr";

    case SP_ID_ORE_PROC:
      return "oreproc.spr";

    case SP_ID_MINING:
      return "mining.spr";

    case SP_ID_EXCHANGE:
      return "exchange.spr";

    case SP_ID_ITEM_MAKE:
      return "itemmake.spr";

    case SP_ID_ITEM_UPGRADE:
      return "itemupgrade.spr";

    case SP_ID_TRADE:
      return "trade.spr";

    case SP_ID_MAPLOADING:
      return "maploading.spr";

    case SP_ID_DUNGEON:
      return "dungeon.spr";

    case SP_ID_UNIT:
      return "unit.spr";

    case SP_ID_UNIT_PARTS:
      return "unitparts.spr";

    case SP_ID_CLASS:
      return "class.spr";

    default:
      break;
  }

  return NULL;
}

BOOL BuildSpriteFileName(WORD pi_wSpriteID, char *po_pFileName, size_t pi_nFileNameSize)
{
  if (!po_pFileName || !pi_nFileNameSize)
  {
    return FALSE;
  }

  const char *l_pBaseFileName = GetSpriteBaseFileName(pi_wSpriteID);
  if (!l_pBaseFileName)
  {
    po_pFileName[0] = '\0';
    return FALSE;
  }

  const char *l_pPrimaryFolder = GetClientNationFolder();
  if (l_pPrimaryFolder && l_pPrimaryFolder[0])
  {
    sprintf_s(po_pFileName, pi_nFileNameSize, ".\\SpriteImage\\%s\\%s", l_pPrimaryFolder, l_pBaseFileName);
    if (IsExistingFile(po_pFileName))
    {
      return TRUE;
    }
  }

  sprintf_s(po_pFileName, pi_nFileNameSize, ".\\SpriteImage\\Common\\%s", l_pBaseFileName);
  if (IsExistingFile(po_pFileName))
  {
    return TRUE;
  }

  sprintf_s(po_pFileName, pi_nFileNameSize, ".\\SpriteImage\\%s", l_pBaseFileName);
  return IsExistingFile(po_pFileName);
}
}

CSpriteUnit::CSpriteUnit()
  : m_wID(0),
    m_pSprite(NULL)
{
}

CSpriteUnit::~CSpriteUnit()
{
  Destroy();
}

void CSpriteUnit::Create(WORD pi_wSpriteID, char *pi_pFileName)
{
  Destroy();

  m_wID = pi_wSpriteID;
  m_pSprite = new CSprite;
  m_pSprite->LoadSprite(pi_pFileName);
}

void CSpriteUnit::Destroy(void)
{
  if (!m_pSprite)
  {
    return;
  }

  m_pSprite->ReleaseSprite();
  delete m_pSprite;
  m_pSprite = NULL;
}

CSpriteMgr::CSpriteMgr()
{
}

CSpriteMgr::~CSpriteMgr()
{
  UnloadSpriteAll();
}

void CSpriteMgr::Init(void)
{
}

void CSpriteMgr::LoadSprite(WORD pi_wSpriteID)
{
  if (GetSprite(pi_wSpriteID))
  {
    return;
  }

  char l_szFileName[MAX_PATH];
  ZeroMemory(l_szFileName, sizeof(l_szFileName));
  if (!BuildSpriteFileName(pi_wSpriteID, l_szFileName, sizeof(l_szFileName)))
  {
    return;
  }

  LoadSprite(pi_wSpriteID, l_szFileName);
}

void CSpriteMgr::LoadSprite(WORD pi_wSpriteID, char *pi_pFilePathName)
{
  if (!pi_pFilePathName || !pi_pFilePathName[0] || GetSprite(pi_wSpriteID))
  {
    return;
  }

  CSpriteUnit *l_pSpriteUnit = new CSpriteUnit;
  l_pSpriteUnit->Create(pi_wSpriteID, pi_pFilePathName);
  m_listSpriteUnit.push_back(l_pSpriteUnit);
}

void CSpriteMgr::UnloadSprite(WORD pi_wSpriteID)
{
  for (CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
  {
    if (!(*i) || (*i)->m_wID != pi_wSpriteID)
    {
      continue;
    }

    CSpriteUnit *l_pUnit = *i;
    m_listSpriteUnit.erase(i);
    delete l_pUnit;
    return;
  }
}

void CSpriteMgr::UnloadSpriteAll(void)
{
  for (CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
  {
    delete *i;
  }

  m_listSpriteUnit.clear();
}

CSprite *CSpriteMgr::GetSprite(WORD pi_wSpriteID)
{
  for (CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
  {
    if (!(*i) || (*i)->m_wID != pi_wSpriteID)
    {
      continue;
    }

    return (*i)->m_pSprite;
  }

  return NULL;
}

CSprite *CSpriteMgr::GetSprite(WORD pi_wSpriteID) const
{
  for (CSpriteUnitList::const_iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
  {
    if (!(*i) || (*i)->m_wID != pi_wSpriteID)
    {
      continue;
    }

    return (*i)->m_pSprite;
  }

  return NULL;
}
