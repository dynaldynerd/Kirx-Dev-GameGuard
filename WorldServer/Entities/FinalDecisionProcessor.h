#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class __cppobj __declspec(align(2)) FinalDecisionProcessor : public ElectProcessor
{
public:
  FinalDecisionProcessor();

  _pt_notify_final_decision _Send[3];
};
