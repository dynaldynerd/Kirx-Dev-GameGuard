#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _monster_fld;
class CMapData;
class CMonster;

struct  _event_respawn
{
  struct _mon
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

  struct _reward_item
  {
    unsigned __int8 byItemTableCode;
    _base_fld *pItemFld;
    _monster_fld *pDstMonFld;
    int nProb;
  };

  struct  _state
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

    _state();
    void init();
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

  _event_respawn();
};

class  CMonsterEventRespawn
{
public:
  CMonsterEventRespawn();

  bool SetEventRespawn();
  void CheckRespawnEvent();
  bool StartRespawnEvent(char *pszEventCode, char *pwszErrCode);
  bool StopRespawnEvent(char *pszEventCode, char *pwszErrCode);

  virtual ~CMonsterEventRespawn();

  int m_nLoadEventRespawn;
  _event_respawn m_EventRespawn[32];
};

extern CMonsterEventRespawn g_MonsterEventRespawn;

