#pragma once

#include "IdaCompat.h"
#include "CTotalGuildRankInfo.h"
#include "total_guild_rank_info.h"

class CPlayer;

class __cppobj CTotalGuildRankManager
{
public:
  CTotalGuildRankManager();
  ~CTotalGuildRankManager();

  static CTotalGuildRankManager *Instance();
  bool Init();
  bool Load();
  __int64 Load(unsigned __int8 byDayAfter, _total_guild_rank_info *pkInfo);
  void Loop();
  void Send(unsigned int dwVer, unsigned __int8 byTabRace, CPlayer *pkPlayer);
  void OrderRank(_total_guild_rank_info *pkInfo);
  bool Update(_total_guild_rank_info *pLoadData);
  void UpdateComlete(unsigned __int8 byRet, _total_guild_rank_info *pLoadData);

  int m_iOldDay;
  CTotalGuildRankInfo m_kInfo;
};
