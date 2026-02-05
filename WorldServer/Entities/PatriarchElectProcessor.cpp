#include "pch.h"

#include "PatriarchElectProcessor.h"

PatriarchElectProcessor *PatriarchElectProcessor::Instance()
{
  static PatriarchElectProcessor s_instance;
  return &s_instance;
}

bool PatriarchElectProcessor::Initialize()
{
  return true;
}

unsigned int PatriarchElectProcessor::GetCurrPatriarchElectSerial()
{
  return _dwCurrPatriarchElectSerial;
}
