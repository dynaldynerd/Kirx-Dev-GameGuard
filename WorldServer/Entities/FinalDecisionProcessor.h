#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "ElectPacketTypes.h"

class __cppobj __declspec(align(2)) FinalDecisionProcessor : public ElectProcessor
{
public:
  FinalDecisionProcessor();
  bool Initialize() override;
  int Doit(Cmd cmd, CPlayer *player, char *data) override;

  _pt_notify_final_decision _Send[3];

private:
  void _FinalDecision();
  void _SetWinner();
  void _ReqNetFinalDecision(CPlayer *player);
};
