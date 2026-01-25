#include "pch.h"

#include "TimeItem.h"

TimeItem *TimeItem::Instance()
{
  static TimeItem s_instance;
  return &s_instance;
}

bool TimeItem::Init()
{
  return true;
}
