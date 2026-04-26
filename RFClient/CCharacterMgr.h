#pragma once

#include <list>

#include <windows.h>

#include "CObject.h"
#include "Character.h"

class CCharacter;
class CPlayer;
class CBaseNpc;
class CMonster;

class CCharacterMgr
{
public:
  typedef std::list<CCharacter *> CHAR_LIST;
  typedef std::list<CCharacter *>::iterator CHAR_LIST_ITER;

private:
  static ChInterface *s_pCharIF;
  static AnimationManager *s_pAnimationMgr;
  static TextureManager *s_pTextureMgr;
  static CObjectManager *s_pBoneMgr[MAX_CHAR_TYPE];
  static CObjectManager *s_pMeshMgr[MAX_CHAR_TYPE];

  bool m_bInitialized;
  CHAR_LIST m_listActive[MAX_CHAR_TYPE];
  CHAR_LIST m_listCache[MAX_CHAR_TYPE];

public:
  CCharacterMgr();
  ~CCharacterMgr();

  bool Initialize(IDirect3DDevice8 *pi_pD3DDevice);
  void Shutdown(void);
  void Clear(void);
  void ClearCharacter(DWORD pi_dwCharTypeID);

  CCharacter *AddCharacter(DWORD pi_dwCharTypeID, CCharacter *pi_pCharacter);
  CPlayer *AddPlayer(DWORD pi_dwIndex);
  CMonster *AddMonster(DWORD pi_dwIndex);
  CBaseNpc *AddNpc(DWORD pi_dwIndex);

  CCharacter *GetCharacter(DWORD pi_dwCharTypeID, DWORD pi_dwIndex);
  CPlayer *GetPlayer(DWORD pi_dwIndex);
  CMonster *GetMonster(DWORD pi_dwIndex);
  CBaseNpc *GetNpc(DWORD pi_dwIndex);

  BOOL FrameMove(void);
  BOOL Animation(void);
  BOOL Render(void);

  static ChInterface *GetCharIF(void);
  static AnimationManager *GetAnimationMgr(void);
  static TextureManager *GetTextureMgr(void);
  static CObjectManager *GetBoneMgr(DWORD pi_dwCharTypeID);
  static CObjectManager *GetMeshMgr(DWORD pi_dwCharTypeID);
};
