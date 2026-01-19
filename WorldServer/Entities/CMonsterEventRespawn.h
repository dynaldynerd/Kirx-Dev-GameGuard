#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _monster_fld;
class CMapData;
class CMonster;

struct __cppobj _event_respawn
{
  struct __declspec(align(8)) _mon
  {
    _base_fld *pMonsterFld;
    unsigned __int16 wNum;
  };

  struct _option
  {
    bool bKillAfterStop;
    bool bExpPenalty;
    bool bExpReward;
    bool bItemLoot;
  };

  struct __declspec(align(8)) _reward_item
  {
    unsigned __int8 byItemTableCode;
    _base_fld *pItemFld;
    _monster_fld *pDstMonFld;
    int nProb;
  };

  struct __cppobj _state
  {
    struct _mon
    {
      CMonster *pMon;
      unsigned int dwSerial;
      _base_fld *pMonFld;
    };

    unsigned int dwLastUpdateTime;
    int nRespawnNum;
    _mon MonInfo[640];
  };

  bool bLoad;
  unsigned __int16 wMonSetNum;
  _mon MonSet[16];
  CMapData *pMap;
  float fPos[3];
  unsigned int dwTermMSec;
  _option Option;
  int nUseRewardItemNum;
  _reward_item RewardItem[128];
  char szScriptName[64];
  bool bActive;
  _state State;
};

class __cppobj CMonsterEventRespawn
{
public:
  virtual ~CMonsterEventRespawn() = default;

  int m_nLoadEventRespawn;
  _event_respawn m_EventRespawn[32];
};
