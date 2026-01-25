#include "pch.h"

#include "TimeLimitJadeMng.h"

TimeLimitJadeMng *TimeLimitJadeMng::Instance()
{
  static TimeLimitJadeMng s_instance;
  return &s_instance;
}

bool TimeLimitJadeMng::Init()
{
  return true;
}
