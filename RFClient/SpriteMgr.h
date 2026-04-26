#pragma once

#include <list>

#include <windows.h>

#include "R3Engine/2ndclass/2dsprite.h"

#define SP_ID_MOUSE         0x00
#define SP_ID_COMMON        0x01
#define SP_ID_GAUGE_EXP     0x02
#define SP_ID_BELT          0x03
#define SP_ID_INVENTORY     0x04
#define SP_ID_CHAT          0x05
#define SP_ID_SHOP          0x06
#define SP_ID_LOGIN         0x07
#define SP_ID_LOADING       0x08
#define SP_ID_ITEM          0x09
#define SP_ID_ORE_PROC      0x0A
#define SP_ID_MINING        0x0B
#define SP_ID_EXCHANGE      0x0C
#define SP_ID_RECALL        0x0D
#define SP_ID_ITEM_MAKE     0x0E
#define SP_ID_ITEM_UPGRADE  0x0F
#define SP_ID_PARTY         0x10
#define SP_ID_TRADE         0x11
#define SP_ID_MAPLOADING    0x12
#define SP_ID_DUNGEON       0x13
#define SP_ID_UNIT          0x14
#define SP_ID_UNIT_PARTS    0x15
#define SP_ID_CLASS         0x16
#define SP_ID_MINIMAP       0x17

class CSpriteUnit
{
public:
  WORD m_wID;
  CSprite *m_pSprite;

public:
  CSpriteUnit();
  ~CSpriteUnit();

  void Create(WORD pi_wSpriteID, char *pi_pFileName);
  void Destroy(void);
};

class CSpriteMgr
{
public:
  typedef std::list<CSpriteUnit *> CSpriteUnitList;

private:
  CSpriteUnitList m_listSpriteUnit;

public:
  CSpriteMgr();
  ~CSpriteMgr();

  void Init(void);
  void LoadSprite(WORD pi_wSpriteID);
  void LoadSprite(WORD pi_wSpriteID, char *pi_pFilePathName);
  void UnloadSprite(WORD pi_wSpriteID);
  void UnloadSpriteAll(void);

  CSprite *GetSprite(WORD pi_wSpriteID);
  CSprite *GetSprite(WORD pi_wSpriteID) const;
};
