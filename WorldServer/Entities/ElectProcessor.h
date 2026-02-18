#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class CPlayer;

enum Cmd : int
{
  _eRequestInitCandidate = 0x0,
  _eReqUpdateWinCount = 0x1,
  _eRequestCandidateList = 0x2,
  _eRegCandidate = 0x3,
  _eReg2ndCandidate = 0x4,
  _eMakeVotePaper = 0x5,
  _eSendVotePaper = 0x6,
  _eVote = 0x7,
  _eVoteScore = 0x8,
  _eReqQryFinalDecision = 0x9,
  _eReqQrySetWinner = 0xA,
  _eReqQryFinalApplyer = 0xB,
  _eRetQryFinalDecision = 0xC,
  _eReqNetFinalDecision = 0xD,
  _eQryAppoint = 0xE,
  _eReqAppoint = 0xF,
  _eRespAppoint = 0x10,
  _eReqDischarge = 0x11,
  _eReqPatriarchInform = 0x12,
  _eNon = 0x13,
};

class  ElectProcessor
{
public:
  enum ProcessorType : int
  {
    _eCandidateRegister = 0x0,
    _eSecondCandidateCrystallizer = 0x1,
    _eVoter = 0x2,
    _eFinalDecisionProcessor = 0x3,
    _eFinalDecisionApplyer = 0x4,
    _eClassOrderProcessor = 0x5,
    _eProcessorNum = 0x6,
    _eNonProcessor = 0xFF,
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
