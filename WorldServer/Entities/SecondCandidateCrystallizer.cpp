#include "pch.h"

#include "SecondCandidateCrystallizer.h"
#include "PatriarchElectProcessor.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"

#include <cstdio>

SecondCandidateCrystallizer::SecondCandidateCrystallizer()
  : ElectProcessor(_eSecondCandidateCrystallizer)
{
}

bool SecondCandidateCrystallizer::Initialize()
{
  char logPath[256]{};
  std::snprintf(
    logPath,
    sizeof(logPath),
    "..\\ZoneServerLog\\SystemLog\\Patriarch\\SecondCandidateCrystallizer_%llu.log",
    GetKorLocalTime());
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  ElectProcessor::Initialize();
  Doit(_eReg2ndCandidate, nullptr, nullptr);
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x78u, nullptr, false);
  return true;
}

int SecondCandidateCrystallizer::Doit(Cmd cmd, CPlayer *player, char *data)
{
if (cmd == _eReg2ndCandidate)
  {
    PatriarchElectProcessor::Instance()->PushDQSCheckInvalidChar();
    return 0;
  }

  return 255;
}

