#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "ElectProcessor.h"

class __cppobj PatriarchElectProcessor
{
public:
  static PatriarchElectProcessor *Instance();
  bool Initialize();
  unsigned int GetCurrPatriarchElectSerial();

  unsigned int m_dwNonvoteCnt[3];
  unsigned int m_dwTotalVoteCnt[3];
  unsigned int m_dwHighGradeNum[3];
  ElectProcessor::ProcessorType _eProcessType;
  ElectProcessor *_kRunningProcessor;
  ElectProcessor *_kProcessor[6];
  bool _bTimeCheck;
  bool _bInitProce;
  unsigned int _dwNextCheckTime;
  unsigned int _dwNextCheckDay;
  unsigned int _dwNextScoreUpdateTime;
  unsigned int _dwElectSerial;
  unsigned int _dwCurrPatriarchElectSerial;
  CLogFile _kSysLog;
};
