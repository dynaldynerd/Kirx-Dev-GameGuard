#include "pch.h"

#include "LendItemMng.h"

LendItemMng *LendItemMng::Instance()
{
  static LendItemMng s_instance;
  return &s_instance;
}

bool LendItemMng::Initialize()
{
  return true;
}
