#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class __cppobj __declspec(align(2)) Voter : public ElectProcessor
{
public:
  Voter();

  _pt_trans_votepaper_zocl _kCandidateInfo[3];
  _pt_notify_vote_score_zocl _kVoteScoreInfo[3];
};
