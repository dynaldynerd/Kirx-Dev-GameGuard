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

unsigned __int8 CGoldenBoxItemMgr::Get_Event_Status()
{
  return m_golden_box_event.m_event_status;
}
