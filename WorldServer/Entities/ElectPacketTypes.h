#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(1)) _pt_trans_votepaper_zocl
{
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    char wszGuildName[17];
    unsigned int dwWinCnt;
  };

  _pt_trans_votepaper_zocl();
  __int64 size() const;

  unsigned __int8 byCnt;
  __body body[8];
};

struct _pt_notify_vote_score_zocl
{
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    unsigned __int8 byScoreRate;
  };

  _pt_notify_vote_score_zocl();
  __int64 size() const;

  unsigned __int8 byRace;
  unsigned __int8 byVoteRate;
  unsigned __int8 byNonvoteRate;
  unsigned __int8 byCnt;
  __body body[8];
};

struct _pt_notify_final_decision
{
  __int64 size() const;

  char wszAvatorName[5][17];
};

struct __unaligned __declspec(align(1)) _pt_appoint_inform_request_zocl
{
  struct __body
  {
    unsigned __int8 byLevel;
    unsigned __int8 byClassType;
    long double dPvpPoint;
    char wszAvatorName[17];
  };

  __body body[4];
};

struct __cppobj _pt_query_appoint_zocl
{
  unsigned __int8 byClassType;
  unsigned __int8 byRet;
  char wszAvatorName[17];

  _pt_query_appoint_zocl();
  __int64 size();
};

inline _pt_query_appoint_zocl::_pt_query_appoint_zocl()
{
  memset_0(this, 0, sizeof(*this));
}

inline __int64 _pt_query_appoint_zocl::size()
{
  return 19;
}

inline _pt_trans_votepaper_zocl::_pt_trans_votepaper_zocl()
{
  memset_0(this, 0, sizeof(*this));
}

inline __int64 _pt_trans_votepaper_zocl::size() const
{
  return 313 - 39LL * (8 - byCnt);
}

inline _pt_notify_vote_score_zocl::_pt_notify_vote_score_zocl()
{
  memset_0(this, 0, sizeof(*this));
}

inline __int64 _pt_notify_vote_score_zocl::size() const
{
  return 156 - 19LL * (8 - byCnt);
}

inline __int64 _pt_notify_final_decision::size() const
{
  return 85;
}
