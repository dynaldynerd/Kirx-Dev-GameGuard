#include "pch.h"

#include "CGoldenBoxItemMgr.h"

CGoldenBoxItemMgr *CGoldenBoxItemMgr::Instance()
{
  static CGoldenBoxItemMgr s_instance;
  return &s_instance;
}

bool CGoldenBoxItemMgr::Initialize()
{
  return true;
}
