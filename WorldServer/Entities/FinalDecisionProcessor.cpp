#include "pch.h"

#include "FinalDecisionProcessor.h"

#include <cstring>

FinalDecisionProcessor::FinalDecisionProcessor()
  : ElectProcessor(_eFinalDecisionProcessor)
{
  memset(_Send, 0, sizeof(_Send));
}
