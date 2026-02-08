#include "pch.h"

#include "MyTimer.h"

MyTimer::TIME MyTimer::_time;

MyTimer::TIME::TIME()
{
  _nYear = 1900;
  _nMonth = 1;
  _nDay = 1;
  _nHour = 0;
  _nMin = 0;
  _nSec = 0;
}

const MyTimer::TIME *MyTimer::GetTime()
{
  return &MyTimer::_time;
}
