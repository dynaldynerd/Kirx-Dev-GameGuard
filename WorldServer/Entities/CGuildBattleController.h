#pragma once

#include "IdaCompat.h"

class CGuild;
class CPlayer;

class __cppobj CGuildBattleController
{
public:
  static CGuildBattleController *Instance();
  bool Init();
  unsigned __int8 Add(
    CGuild *pSrcGuild,
    CGuild *pDestGuild,
    unsigned int dwStartTime,
    unsigned __int8 byNumber,
    unsigned int dwMapInx);
  unsigned __int8 Add(
    CGuild *pSrcGuild,
    CGuild *pDestGuild,
    unsigned int dwStartTime,
    unsigned int dwElapseTimeCnt,
    unsigned __int8 byNumber,
    unsigned int dwMapInx);
  void LogIn(CPlayer *pkPlayer);
  void SendPossibleBattleGuildListFirst(int n, unsigned __int8 byRace);

  virtual ~CGuildBattleController() = default;
};

