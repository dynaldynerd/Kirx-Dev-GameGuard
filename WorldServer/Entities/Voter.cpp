#include "pch.h"

#include "Voter.h"

#include <cstring>

Voter::Voter()
  : ElectProcessor(_eVoter)
{
  memset(_kCandidateInfo, 0, sizeof(_kCandidateInfo));
  memset(_kVoteScoreInfo, 0, sizeof(_kVoteScoreInfo));
}
