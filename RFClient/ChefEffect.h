#pragma once

#include <windows.h>

#include <string>

#include "Character.h"
#include "R3Engine/1stclass/r3particle.h"

class EffectFileList
{
public:
  void LoadEffectFileList(char *pi_pFileName);
};

class EffectPatternList
{
public:
  void LoadEffectPatternList(char *pi_pFileName);
};

class ItemEffectList
{
public:
  void LoadItemListFile(char *pi_pFileName);
};

class EffectList
{
public:
  void LoadEffectFileList(char *pi_pFileName);
};

class EffectObjectManager
{
public:
  EffectObjectManager(void);
  static void LoadEffect(char *pi_pFileName);
  static int SetPartEffect(CHARACTEROBJECT *pi_pCharacterObject,
                           char *pi_pFileName,
                           int pi_nLevel);
  static EffectObjectManager m_This;
};

struct CHEF_EFFECT_ATTACHMENT
{
  DWORD m_ID;
  float m_X;
  float m_Y;
  float m_Z;
};

struct CHPARTICLE
{
  BOOL m_bLoad;
  BOOL m_bRealLoad;
  CParticle m_Particle[5];
  BOOL m_bAbsAxis;
  float m_fScale[3];
  std::string m_sFileName;
  DWORD m_Count;

  CHPARTICLE(void);
  void Release(void);
  BOOL RealLoad(void);
};

struct CHENTITY
{
  BOOL m_bLoad;
  BOOL m_bRealLoad;
  CEntity m_Entity;
  BOOL m_bAbsAxis;
  float m_fScale[3];
  float m_fRot[3];
  std::string m_sFileName;

  CHENTITY(void);
  void Release(void);
  BOOL RealLoad(void);
};

class CParticleManager
{
public:
  CParticleManager(void);
  ~CParticleManager(void);

  CParticle *GetParticle(DWORD pi_dwID);
  CEntity *GetEntity(DWORD pi_dwID);
  CHPARTICLE *GetCHParticle(DWORD pi_dwID);
  CHENTITY *GetCHEntity(DWORD pi_dwID);
  BOOL LoadParticleIni(char *pi_pFileName);
  BOOL LoadEntityIni(char *pi_pFileName);
  void ReleaseParticle(void);
  void ReleaseEntity(void);
  void ResetParticleCount(void);

private:
  void AddParticle(char *pi_pFileName, DWORD pi_dwID, CHPARTICLE pi_stParticle);
  void AddEntity(char *pi_pFileName, DWORD pi_dwID, CHENTITY pi_stEntity);

private:
  DWORD m_Counter;
  CHPARTICLE m_Particle[1024];
  CHENTITY m_Entity[1024];
};

bool InitializeChefEffectsOnce(void);
void ApplyMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                     DWORD pi_dwMeshID,
                     DWORD pi_dwLevel);
bool HasMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                   DWORD pi_dwMeshID,
                   DWORD pi_dwLevel);
void DrawCharacterctEffect(CHARACTEROBJECT *pi_pCharacterObject,
                           float pi_afPos[3],
                           float pi_fRotY,
                           float pi_fAlpha,
                           float pi_fScale = 1.0f);
void SetCharacterctEffectArgv(DWORD pi_dwID, Vector3f pi_vecPos);
void ResetChefParticleCount(void);
