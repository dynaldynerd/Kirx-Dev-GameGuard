#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class CPlayer;

enum Cmd : int
{
  _eRequestInitCandidate = 0,
  _eReqUpdateWinCount = 1,
  _eRequestCandidateList = 2,
  _eRegCandidate = 3,
  _eReg2ndCandidate = 4,
  _eMakeVotePaper = 5,
  _eSendVotePaper = 6,
  _eVote = 7,
  _eVoteScore = 8,
  _eReqQryFinalDecision = 9,
  _eReqQrySetWinner = 10,
  _eReqQryFinalApplyer = 11,
  _eRetQryFinalDecision = 12,
  _eReqNetFinalDecision = 13,
  _eQryAppoint = 14,
  _eReqAppoint = 15,
  _eRespAppoint = 16,
  _eReqDischarge = 17,
  _eReqPatriarchInform = 18,
  _eNon = 19,
};

class  ElectProcessor
{
public:
  enum ProcessorType : int
  {
    _eCandidateRegister = 0,
    _eSecondCandidateCrystallizer = 1,
    _eVoter = 2,
    _eFinalDecisionProcessor = 3,
    _eFinalDecisionApplyer = 4,
    _eClassOrderProcessor = 5,
    _eProcessorNum = 6,
    _eNonProcessor = 255,
  };

  ElectProcessor(ProcessorType nProcessorType);
  virtual ~ElectProcessor() = default;
  virtual bool Initialize();
  virtual int Doit(Cmd cmd, CPlayer *pkPlayer, char *pszData);
  ProcessorType GetProcessorType();

  bool _bEnable;
  ProcessorType _nProcesor;
  CLogFile _kSysLog;
};
