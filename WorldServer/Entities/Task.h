#pragma once

#include "IdaCompat.h"
#include "MyTimer.h"

class  Task
{
public:
  Task();
  ~Task();

  bool Initialize(unsigned __int64 nMaxBufSize);
  void SetTask(int nTaskCode, const unsigned __int8 *p, unsigned __int64 size);
  unsigned __int8 *GetTaskBuf();
  int GetTaskCode();
  void SetRetCode(int nCode);
  int GetRetCode();

  bool _bFill;
  int _nRetCode;
  int _nTaskCode;
  MyTimer::TIME _tmReg;
  unsigned __int8 *_pBuf;
  unsigned __int64 _nBufSize;
  unsigned __int64 _nMaxBufSize;
};
