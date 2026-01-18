#pragma once

#include "IdaCompat.h"

/* 1524 */
struct __cppobj __declspec(align(8)) CDummyPosTable
{
  _dummy_position *m_pDumPos;
  int m_nDumPosDataNum;
  virtual ~CDummyPosTable() = default;
};
