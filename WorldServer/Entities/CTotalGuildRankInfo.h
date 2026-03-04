#pragma once

#include "IdaCompat.h"
#include "total_guild_rank_info.h"

struct _total_guild_rank_result_zocl;

class  CTotalGuildRankRecord
{
public:
  CTotalGuildRankRecord();
  ~CTotalGuildRankRecord();
  void Clear();

  unsigned int m_dwInx;
  unsigned __int16 m_wRank;
  unsigned int m_dwSerial;
  char m_wszGuildName[17];
  unsigned __int8 m_byGrade;
  char m_wszMasterName[17];
};

#pragma pack(push, 1)
struct  _total_guild_rank_result_zocl
{
  #pragma pack(push, 1)
  struct _list
  {
    unsigned __int8 byRank;
    char wszGuildName[17];
    unsigned __int8 byGrade;
    char wszMasterName[17];
  };
  #pragma pack(pop)

  unsigned int dwVer;
  unsigned __int8 byRace;
  unsigned __int8 byExistSelfRankInfo;
  unsigned __int8 byCnt;
  _list list[11];

  __int64 size();
};
#pragma pack(pop)

static_assert(sizeof(_total_guild_rank_result_zocl) == 403, "_total_guild_rank_result_zocl size mismatch");

class  CTotalGuildRankInfo
{
public:
  CTotalGuildRankInfo();
  ~CTotalGuildRankInfo();
  bool Init();
  void Clear();
  __int64 Find(unsigned __int8 byRace, unsigned int dwGuildSerial);
  char Load(_total_guild_rank_info *pkInfo);
  void SetNoDataFlag();
  void Send(
    unsigned int dwVer,
    unsigned int n,
    unsigned __int8 byTabRace,
    unsigned __int8 bySelfRace,
    unsigned int dwGuildSerial);
  bool Update(_total_guild_rank_info *pkInfo);

  bool m_bInit;
  bool m_bNoData;
  unsigned int m_dwRecordCnt[3];
  CTotalGuildRankRecord **m_ppkRaceStartPos[3];
  unsigned int m_dwMaxCnt;
  CTotalGuildRankRecord **m_ppkInfo;
  _total_guild_rank_result_zocl *m_pkSendList;
};
