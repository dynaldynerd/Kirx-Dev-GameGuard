#include "pch.h"

#include "ElectProcessor.h"

ElectProcessor::ElectProcessor(ProcessorType nProcessorType)
  : _nProcesor(nProcessorType)
{
}

bool ElectProcessor::Initialize()
{
  _bEnable = true;
  return true;
}

int ElectProcessor::Doit(Cmd cmd, CPlayer *pkPlayer, char *pszData)
{
  return 0;
}

ElectProcessor::ProcessorType ElectProcessor::GetProcessorType()
{
  return _nProcesor;
}
