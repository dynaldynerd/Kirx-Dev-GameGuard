#pragma once

#include "IdaCompat.h"

/* 1264 */
struct __cppobj CMsgData
{
  int m_nObjNum;
  int m_nMaxBufNum;
  _message m_gmListHead;
  _message m_gmListTail;
  _message *m_gmBuf;
  _message m_gmListEmptyHead;
  _message m_gmListEmptyTail;
  CMyCriticalSection m_csList;
  CMyCriticalSection m_csEmpty;
  virtual ~CMsgData() = default;
};
