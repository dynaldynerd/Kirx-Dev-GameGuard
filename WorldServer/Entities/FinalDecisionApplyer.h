#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"

class __cppobj FinalDecisionApplyer : public ElectProcessor
{
public:
  FinalDecisionApplyer();
  bool Initialize() override;
  int Doit(Cmd cmd, CPlayer *player, char *data) override;

private:
  void _FinalDecisionApply();
};
