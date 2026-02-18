#pragma once

#include "IdaCompat.h"

class TaskPool;

class  Worker
{
public:
  Worker(const char *pWorkerName, int nMaxTskNum, int nMaxTskBufSize);
  virtual ~Worker();

  virtual void DoWork() = 0;

  bool Initialize(int nLoop, int nTerm);
  void Start();
  void Stop();
  static void __cdecl s_loop(void *pArg);

  bool _bInit;
  bool _bRun;
  int _nLoop;
  int _nTerm;
  int _nMaxTskNum;
  int _nMaxTskBufSize;
  char _szWorkerName[128];
  void *_hRunEvent;
  TaskPool *_pkPool;
};
