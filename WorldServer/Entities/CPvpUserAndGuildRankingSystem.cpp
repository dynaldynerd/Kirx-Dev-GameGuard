#include "pch.h"

#include "CPvpUserAndGuildRankingSystem.h"

CPvpUserAndGuildRankingSystem *CPvpUserAndGuildRankingSystem::Instance()
{
  static CPvpUserAndGuildRankingSystem s_instance;
  return &s_instance;
}

bool CPvpUserAndGuildRankingSystem::Init()
{
  return true;
}
