#pragma once

#include "IdaCompat.h"

struct _base_fld;
class CMapData;
class CMonster;
class CPlayer;

#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME
{
  unsigned int dwLowDateTime;
  unsigned int dwHighDateTime;
} FILETIME;
#endif

struct  __declspec(align(8)) _event_set
{
  struct _monster_set
  {
    struct _state
    {
      struct _mon
      {
        CMonster *pMon;
        unsigned int dwSerial;
        _base_fld *pMonFld;
      };

      unsigned int dwStartTime;
      unsigned int dwLastUpdateTime;
      int nRespawnNum;
      bool bOper;
      _mon MonInfo[100];
    };

    CMapData *pMap;
    float fPos[3];
    _base_fld *pMonsterFld;
    unsigned __int16 wNum;
    unsigned int dwRegenTerm;
    unsigned __int8 byRegenProb;
    unsigned int dwDuring;
    bool bUnknownMap;
    bool bIsSet;
    _state m_State;
  };

  char m_strId[64];
  _monster_set m_MonSet[10];
  bool m_bOper;
};

struct  _event_set_looting
{
  struct __declspec(align(2)) _event_item
  {
    char strCode[64];
    unsigned __int16 wDropCount;
    unsigned __int16 wDuration;
    unsigned __int8 byProb;
  };

  char strCode[64];
  unsigned __int16 wMagnifications;
  unsigned __int16 wRange;
  int bWithHolyScanner;
  unsigned __int8 byLootAuth;
  _event_item stEventItemList[50];
  int nItemCount;
};

class  __declspec(align(8)) CMonsterEventSet
{
public:
  bool IsINIFileChanged(const char *pszFileName, _FILETIME ftCurr);
  bool StartEventSet(char *pszEventCode, char *pwszErrCode, CPlayer *pOne);
  bool StopEventSet(char *pszEventCode, char *pwszErrCode);
  void CheckEventSetRespawn();
  bool LoadEventSet(char *errBuffer);
  bool LoadEventSetLooting();
  _event_set_looting *GetEvenSetLooting(const char *pszCode);

  virtual ~CMonsterEventSet() = default;

  _event_set m_EventSet[10];
  _event_set_looting m_EventSetLootingList[100];
  _FILETIME m_ftWrite;
  _FILETIME m_ftLootingWrite;
  bool m_bLoadEventLooting;
};

extern CMonsterEventSet g_MonsterEventSet;
