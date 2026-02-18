#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"

class  SecondCandidateCrystallizer : public ElectProcessor
{
public:
  SecondCandidateCrystallizer();
  bool Initialize() override;
  int Doit(Cmd cmd, CPlayer *player, char *data) override;
};
