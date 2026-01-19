#pragma once

#include "IdaCompat.h"

class CMyTimer;

class __cppobj CUnmannedTraderLazyCleaner
{
public:
  CMyTimer *m_pkTimer;
  bool m_bClearProcess;
  unsigned int m_uiRetryCnt;
};
