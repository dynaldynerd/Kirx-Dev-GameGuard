#include "pch.h"

#include "PatriarchElectProcessor.h"

#include "CandidateRegister.h"
#include "SecondCandidateCrystallizer.h"
#include "Voter.h"
#include "FinalDecisionProcessor.h"
#include "FinalDecisionApplyer.h"
#include "ClassOrderProcessor.h"
#include "WorldServerUtil.h"

#include <cstdio>
#include <cstring>
#include <mmsystem.h>

PatriarchElectProcessor *PatriarchElectProcessor::Instance()
{
  static PatriarchElectProcessor s_instance;
  return &s_instance;
}

bool PatriarchElectProcessor::Initialize()
{
  char buffer[260];

  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog\\Patriarch", 0x1E);

  memset(buffer, 0, 256);
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "..\\ZoneServerLog\\SystemLog\\Patriarch\\PatriarchElect_%d.log", now);
  _kSysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\ServiceLog\\Patriarch", 0x1E);

  CandidateRegister *candidate = static_cast<CandidateRegister *>(operator new(0x11438uLL));
  if (candidate)
  {
    new (candidate) CandidateRegister();
  }
  else
  {
    candidate = nullptr;
  }
  _kProcessor[0] = candidate;

  SecondCandidateCrystallizer *secondCandidate =
    static_cast<SecondCandidateCrystallizer *>(operator new(0xC8uLL));
  if (secondCandidate)
  {
    new (secondCandidate) SecondCandidateCrystallizer();
  }
  else
  {
    secondCandidate = nullptr;
  }
  _kProcessor[1] = secondCandidate;

  Voter *voter = static_cast<Voter *>(operator new(0x648uLL));
  if (voter)
  {
    new (voter) Voter();
  }
  else
  {
    voter = nullptr;
  }
  _kProcessor[2] = voter;

  FinalDecisionProcessor *finalDecision = static_cast<FinalDecisionProcessor *>(operator new(0x1C8uLL));
  if (finalDecision)
  {
    new (finalDecision) FinalDecisionProcessor();
  }
  else
  {
    finalDecision = nullptr;
  }
  _kProcessor[3] = finalDecision;

  FinalDecisionApplyer *finalApplyer = static_cast<FinalDecisionApplyer *>(operator new(0xC8uLL));
  if (finalApplyer)
  {
    new (finalApplyer) FinalDecisionApplyer();
  }
  else
  {
    finalApplyer = nullptr;
  }
  _kProcessor[4] = finalApplyer;

  ClassOrderProcessor *classOrder = static_cast<ClassOrderProcessor *>(operator new(0x210uLL));
  if (classOrder)
  {
    new (classOrder) ClassOrderProcessor();
  }
  else
  {
    classOrder = nullptr;
  }
  _kProcessor[5] = classOrder;

  for (int index = 0; index < 6; ++index)
  {
    if (_kProcessor[index]->GetProcessorType() != index)
    {
      return false;
    }
  }

  _eProcessType = ElectProcessor::_eNonProcessor;
  _dwNextCheckTime = timeGetTime() + 60000;
  return true;
}

unsigned int PatriarchElectProcessor::GetCurrPatriarchElectSerial()
{
  return _dwCurrPatriarchElectSerial;
}
