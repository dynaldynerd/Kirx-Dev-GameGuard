#pragma once

#include "IdaCompat.h"

struct _total_guild_rank_result_zocl;

class __cppobj CTotalGuildRankRecord
{
public:
  CTotalGuildRankRecord() = default;
  void Clear();

  unsigned int m_dwInx;
  unsigned __int16 m_wRank;
  unsigned int m_dwSerial;
  char m_wszGuildName[17];
  unsigned __int8 m_byGrade;
  char m_wszMasterName[17];
};

class __cppobj __declspec(align(1)) _total_guild_rank_result_zocl
{
public:
  struct _list
  {
    unsigned __int8 byRank;
    char wszGuildName[17];
    unsigned __int8 byGrade;
    char wszMasterName[17];
  };

  unsigned int dwVer;
  unsigned __int8 byRace;
  unsigned __int8 byExistSelfRankInfo;
  unsigned __int8 byCnt;
  _list list[11];
};

class __cppobj CTotalGuildRankInfo
{
public:
  bool Init();
  void Clear();

  bool m_bInit;
  bool m_bNoData;
  unsigned int m_dwRecordCnt[3];
  CTotalGuildRankRecord **m_ppkRaceStartPos[3];
  unsigned int m_dwMaxCnt;
  CTotalGuildRankRecord **m_ppkInfo;
  _total_guild_rank_result_zocl *m_pkSendList;
};
