#include "pch.h"

#include "CTotalGuildRankManager.h"

CTotalGuildRankManager *CTotalGuildRankManager::Instance()
{
  static CTotalGuildRankManager s_instance;
  return &s_instance;
}

bool CTotalGuildRankManager::Init()
{
  return true;
}
