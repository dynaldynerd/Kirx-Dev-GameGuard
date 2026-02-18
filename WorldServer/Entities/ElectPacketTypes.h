#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _pt_trans_votepaper_zocl
{
#pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    char wszGuildName[17];
    unsigned int dwWinCnt;
  };
#pragma pack(pop)

  _pt_trans_votepaper_zocl();
  __int64 size() const;

  unsigned __int8 byCnt;
  __body body[8];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _pt_notify_vote_score_zocl
{
  #pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byRank;
    char wszAvatorName[17];
    unsigned __int8 byScoreRate;
  };
  #pragma pack(pop)

  _pt_notify_vote_score_zocl();
  __int64 size() const;

  unsigned __int8 byRace;
  unsigned __int8 byVoteRate;
  unsigned __int8 byNonvoteRate;
  unsigned __int8 byCnt;
  __body body[8];
};
#pragma pack(pop)

struct _pt_notify_final_decision
{
  __int64 size() const;

  char wszAvatorName[5][17];
};

#pragma pack(push, 1)
struct _pt_appoint_inform_request_zocl
{
#pragma pack(push, 1)
  struct __body
  {
    unsigned __int8 byLevel;
    unsigned __int8 byClassType;
    long double dPvpPoint;
    char wszAvatorName[17];
  };
#pragma pack(pop)

  __body body[4];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct  _pt_query_appoint_zocl
{
  unsigned __int8 byClassType;
  unsigned __int8 byRet;
  char wszAvatorName[17];

  _pt_query_appoint_zocl();
  __int64 size();
};
#pragma pack(pop)

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

static_assert(sizeof(_pt_trans_votepaper_zocl::__body) == 39, "_pt_trans_votepaper_zocl::__body size mismatch");
static_assert(sizeof(_pt_trans_votepaper_zocl) == 313, "_pt_trans_votepaper_zocl size mismatch");
static_assert(sizeof(_pt_notify_vote_score_zocl::__body) == 19, "_pt_notify_vote_score_zocl::__body size mismatch");
static_assert(sizeof(_pt_notify_vote_score_zocl) == 156, "_pt_notify_vote_score_zocl size mismatch");
static_assert(sizeof(_pt_notify_final_decision) == 85, "_pt_notify_final_decision size mismatch");
static_assert(sizeof(_pt_appoint_inform_request_zocl::__body) == 27, "_pt_appoint_inform_request_zocl::__body size mismatch");
static_assert(sizeof(_pt_appoint_inform_request_zocl) == 108, "_pt_appoint_inform_request_zocl size mismatch");
