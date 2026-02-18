#pragma once

#include "IdaCompat.h"

class  CAsyncLogBuffer
{
public:
  enum ALB_STATE
  {
    ALBS_NONE = -1,
    ALBS_WAIT = 0,
    ALBS_SET = 1,
    ALBS_COMPLETE = 2,
  };

  CAsyncLogBuffer();
  ~CAsyncLogBuffer();

  bool Init(int iMaxBufferSize);
  bool Log(const char *pszFileName, const char *szLog, int iLen);
  char *GetFileName();
  int GetLength();
  char *GetStr();

  ALB_STATE m_eState;
  char m_szFileName[260];
  int m_iLen;
  int m_iMaxBuffSize;
  char *m_szBuffer;
};

