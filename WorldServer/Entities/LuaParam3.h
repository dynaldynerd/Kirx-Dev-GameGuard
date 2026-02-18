#pragma once

#include "IdaCompat.h"

struct  LuaParam3
{
  int m_1;
  int m_2;
  int m_3;

  LuaParam3() : m_1(0), m_2(0), m_3(0) {}
  LuaParam3(int a, int b, int c) : m_1(a), m_2(b), m_3(c) {}
};
