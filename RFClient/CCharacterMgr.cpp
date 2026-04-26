#include "CCharacterMgr.h"

#include "CBaseNpc.h"
#include "CCharacter.h"
#include "CMonster.h"
#include "CPlayer.h"

ChInterface *CCharacterMgr::s_pCharIF = NULL;
AnimationManager *CCharacterMgr::s_pAnimationMgr = NULL;
TextureManager *CCharacterMgr::s_pTextureMgr = NULL;
CObjectManager *CCharacterMgr::s_pBoneMgr[MAX_CHAR_TYPE] = {NULL, NULL, NULL, NULL, NULL, NULL};
CObjectManager *CCharacterMgr::s_pMeshMgr[MAX_CHAR_TYPE] = {NULL, NULL, NULL, NULL, NULL, NULL};

CCharacterMgr::CCharacterMgr()
  : m_bInitialized(false)
{
}

CCharacterMgr::~CCharacterMgr()
{
  Shutdown();
}

bool CCharacterMgr::Initialize(IDirect3DDevice8 *pi_pD3DDevice)
{
  if (m_bInitialized)
  {
    return true;
  }

  if (!pi_pD3DDevice)
  {
    return false;
  }

  if (!s_pCharIF)
  {
    s_pCharIF = static_cast<ChInterface *>(::operator new(sizeof(ChInterface)));
    ZeroMemory(s_pCharIF, sizeof(ChInterface));
    s_pCharIF->InitCharacter(pi_pD3DDevice);
    s_pCharIF->SetRender();
    s_pCharIF->AddPartToMM("BODY");
  }

  if (!s_pAnimationMgr)
  {
    s_pAnimationMgr = static_cast<AnimationManager *>(::operator new(sizeof(AnimationManager)));
    ZeroMemory(s_pAnimationMgr, sizeof(AnimationManager));
    s_pAnimationMgr->Init();
  }

  if (!s_pTextureMgr)
  {
    s_pTextureMgr = static_cast<TextureManager *>(::operator new(sizeof(TextureManager)));
    ZeroMemory(s_pTextureMgr, sizeof(TextureManager));
    s_pTextureMgr->Init();
    s_pTextureMgr->m_pD3DDevice = pi_pD3DDevice;
  }

  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    if (!s_pBoneMgr[i])
    {
      s_pBoneMgr[i] = static_cast<CObjectManager *>(::operator new(sizeof(CObjectManager)));
      ZeroMemory(s_pBoneMgr[i], sizeof(CObjectManager));
    }
  }

  s_pMeshMgr[CTI_PLAYER] = s_pCharIF ? s_pCharIF->GetPart("BODY") : NULL;
  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    if (i == CTI_PLAYER)
    {
      continue;
    }

    if (!s_pMeshMgr[i])
    {
      s_pMeshMgr[i] = static_cast<CObjectManager *>(::operator new(sizeof(CObjectManager)));
      ZeroMemory(s_pMeshMgr[i], sizeof(CObjectManager));
    }
  }

  m_bInitialized = s_pCharIF && s_pAnimationMgr && s_pTextureMgr && s_pBoneMgr[CTI_PLAYER] && s_pMeshMgr[CTI_PLAYER];
  return m_bInitialized;
}

void CCharacterMgr::Shutdown(void)
{
  Clear();
  m_bInitialized = false;
}

void CCharacterMgr::Clear(void)
{
  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    ClearCharacter(i);
  }
}

void CCharacterMgr::ClearCharacter(DWORD pi_dwCharTypeID)
{
  if (pi_dwCharTypeID >= MAX_CHAR_TYPE)
  {
    return;
  }

  for (CHAR_LIST_ITER l_iter = m_listActive[pi_dwCharTypeID].begin();
       l_iter != m_listActive[pi_dwCharTypeID].end();
       ++l_iter)
  {
    delete *l_iter;
  }
  m_listActive[pi_dwCharTypeID].clear();

  for (CHAR_LIST_ITER l_iter = m_listCache[pi_dwCharTypeID].begin();
       l_iter != m_listCache[pi_dwCharTypeID].end();
       ++l_iter)
  {
    delete *l_iter;
  }
  m_listCache[pi_dwCharTypeID].clear();
}

CCharacter *CCharacterMgr::AddCharacter(DWORD pi_dwCharTypeID, CCharacter *pi_pCharacter)
{
  if (!pi_pCharacter || pi_dwCharTypeID >= MAX_CHAR_TYPE)
  {
    return NULL;
  }

  pi_pCharacter->SetObjectTypeID(static_cast<BYTE>(pi_dwCharTypeID));
  pi_pCharacter->SetCharTypeID(pi_dwCharTypeID);
  m_listActive[pi_dwCharTypeID].push_back(pi_pCharacter);
  return pi_pCharacter;
}

CPlayer *CCharacterMgr::AddPlayer(DWORD pi_dwIndex)
{
  if (CPlayer *l_pPlayer = GetPlayer(pi_dwIndex))
  {
    return l_pPlayer;
  }

  CPlayer *l_pPlayer = new CPlayer;
  l_pPlayer->SetIndex(pi_dwIndex);
  l_pPlayer->SetName("Player");
  l_pPlayer->Initialize();
  return static_cast<CPlayer *>(AddCharacter(CTI_PLAYER, l_pPlayer));
}

CMonster *CCharacterMgr::AddMonster(DWORD pi_dwIndex)
{
  if (CMonster *l_pMonster = GetMonster(pi_dwIndex))
  {
    return l_pMonster;
  }

  CMonster *l_pMonster = new CMonster;
  l_pMonster->SetIndex(pi_dwIndex);
  l_pMonster->SetName("Monster");
  return static_cast<CMonster *>(AddCharacter(CTI_MONSTER, l_pMonster));
}

CBaseNpc *CCharacterMgr::AddNpc(DWORD pi_dwIndex)
{
  if (CBaseNpc *l_pNpc = GetNpc(pi_dwIndex))
  {
    return l_pNpc;
  }

  CBaseNpc *l_pNpc = new CBaseNpc;
  l_pNpc->SetIndex(pi_dwIndex);
  l_pNpc->SetName("Npc");
  return static_cast<CBaseNpc *>(AddCharacter(CTI_NPC, l_pNpc));
}

CCharacter *CCharacterMgr::GetCharacter(DWORD pi_dwCharTypeID, DWORD pi_dwIndex)
{
  if (pi_dwCharTypeID >= MAX_CHAR_TYPE)
  {
    return NULL;
  }

  for (CHAR_LIST_ITER l_iter = m_listActive[pi_dwCharTypeID].begin();
       l_iter != m_listActive[pi_dwCharTypeID].end();
       ++l_iter)
  {
    if ((*l_iter) && (*l_iter)->IsEqualIndex(pi_dwIndex))
    {
      return *l_iter;
    }
  }

  return NULL;
}

CPlayer *CCharacterMgr::GetPlayer(DWORD pi_dwIndex)
{
  return static_cast<CPlayer *>(GetCharacter(CTI_PLAYER, pi_dwIndex));
}

CMonster *CCharacterMgr::GetMonster(DWORD pi_dwIndex)
{
  return static_cast<CMonster *>(GetCharacter(CTI_MONSTER, pi_dwIndex));
}

CBaseNpc *CCharacterMgr::GetNpc(DWORD pi_dwIndex)
{
  return static_cast<CBaseNpc *>(GetCharacter(CTI_NPC, pi_dwIndex));
}

BOOL CCharacterMgr::FrameMove(void)
{
  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    for (CHAR_LIST_ITER l_iter = m_listActive[i].begin(); l_iter != m_listActive[i].end(); ++l_iter)
    {
      if (*l_iter)
      {
        (*l_iter)->FrameMove();
      }
    }
  }

  return TRUE;
}

BOOL CCharacterMgr::Animation(void)
{
  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    for (CHAR_LIST_ITER l_iter = m_listActive[i].begin(); l_iter != m_listActive[i].end(); ++l_iter)
    {
      if (*l_iter)
      {
        (*l_iter)->Animation();
      }
    }
  }

  return TRUE;
}

BOOL CCharacterMgr::Render(void)
{
  for (int i = 0; i < MAX_CHAR_TYPE; ++i)
  {
    for (CHAR_LIST_ITER l_iter = m_listActive[i].begin(); l_iter != m_listActive[i].end(); ++l_iter)
    {
      if (*l_iter)
      {
        (*l_iter)->Render();
      }
    }
  }

  return TRUE;
}

ChInterface *CCharacterMgr::GetCharIF(void)
{
  return s_pCharIF;
}

AnimationManager *CCharacterMgr::GetAnimationMgr(void)
{
  return s_pAnimationMgr;
}

TextureManager *CCharacterMgr::GetTextureMgr(void)
{
  return s_pTextureMgr;
}

CObjectManager *CCharacterMgr::GetBoneMgr(DWORD pi_dwCharTypeID)
{
  if (pi_dwCharTypeID >= MAX_CHAR_TYPE)
  {
    return NULL;
  }

  return s_pBoneMgr[pi_dwCharTypeID];
}

CObjectManager *CCharacterMgr::GetMeshMgr(DWORD pi_dwCharTypeID)
{
  if (pi_dwCharTypeID >= MAX_CHAR_TYPE)
  {
    return NULL;
  }

  return s_pMeshMgr[pi_dwCharTypeID];
}
