#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class  __declspec(align(2)) Voter : public ElectProcessor
{
public:
  Voter();
  bool Initialize() override;
  int Doit(Cmd cmd, CPlayer *player, char *payload) override;

  _pt_trans_votepaper_zocl _kCandidateInfo[3];
  _pt_notify_vote_score_zocl _kVoteScoreInfo[3];

private:
  void _MakeVotePaper();
  int _SendVotePaper(CPlayer *player);
  int _Vote(CPlayer *player, char *payload);
  void _SendVoteScore(CPlayer *player);
  void _SetVoteScoreInfo(unsigned __int8 raceCode, char *candidateName, bool abstention);
  void _SendVotePaperAll();
  void _SendVoteScoreAll(unsigned __int8 raceCode);
  bool IsRegistedVotePaper(unsigned __int8 raceCode, char *candidateName);
};
