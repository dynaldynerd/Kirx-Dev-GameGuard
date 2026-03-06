#pragma once

#include "IdaCompat.h"
#include "Packet/ZoneClientPacket.h"

struct _pt_notify_final_decision
{
  __int64 size() const;

  char wszAvatorName[5][17];
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

static_assert(sizeof(_pt_trans_votepaper_zocl::__body) == 39, "_pt_trans_votepaper_zocl::__body size mismatch");
static_assert(sizeof(_pt_trans_votepaper_zocl) == 313, "_pt_trans_votepaper_zocl size mismatch");
static_assert(sizeof(_pt_notify_vote_score_zocl::__body) == 19, "_pt_notify_vote_score_zocl::__body size mismatch");
static_assert(sizeof(_pt_notify_vote_score_zocl) == 156, "_pt_notify_vote_score_zocl size mismatch");
static_assert(sizeof(_pt_notify_final_decision) == 85, "_pt_notify_final_decision size mismatch");
static_assert(sizeof(_pt_appoint_inform_request_zocl::__body) == 27, "_pt_appoint_inform_request_zocl::__body size mismatch");
static_assert(sizeof(_pt_appoint_inform_request_zocl) == 108, "_pt_appoint_inform_request_zocl size mismatch");
