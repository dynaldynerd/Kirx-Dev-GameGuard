#include "pch.h"

#include "ElectProcessor.h"

ElectProcessor::ElectProcessor(ProcessorType nProcessorType)
  : _nProcesor(nProcessorType)
{
}

bool ElectProcessor::Initialize()
{
  // this is not a stub
  _bEnable = true;
  return true;
}

int ElectProcessor::Doit(Cmd cmd, CPlayer *pkPlayer, char *pszData)
{
  (void)cmd;
  (void)pkPlayer;
  (void)pszData;
  // this is not a stub
  return 0;
}

ElectProcessor::ProcessorType ElectProcessor::GetProcessorType()
{
  return _nProcesor;
}
