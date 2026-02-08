#include "pch.h"

#include "Worker.h"

#include <new>
#include <process.h>
#include <windows.h>

#include "TaskPool.h"

Worker::Worker(const char *pWorkerName, int nMaxTskNum, int nMaxTskBufSize)
{
  _bInit = false;
  _nMaxTskNum = nMaxTskNum;
  _nMaxTskBufSize = nMaxTskBufSize;
  _hRunEvent = reinterpret_cast<void *>(-1);
  sprintf_s(_szWorkerName, sizeof(_szWorkerName), "%s", pWorkerName);
}

Worker::~Worker()
{
  Stop();
}

bool Worker::Initialize(int nLoop, int nTerm)
{
  _nLoop = nLoop;
  _nTerm = nTerm;
  _pkPool = new (std::nothrow) TaskPool();
  if (!_pkPool)
  {
    return false;
  }
  if (_pkPool->Initialize(_nMaxTskNum, _nMaxTskBufSize))
  {
    return false;
  }
  _bRun = true;
  _hRunEvent = CreateEventA(nullptr, 0, 0, nullptr);
  _beginthread(Worker::s_loop, 0, this);
  _bInit = true;
  return true;
}

void Worker::Start()
{
  SetEvent(_hRunEvent);
}

void Worker::Stop()
{
  if (_pkPool)
  {
    while (!_pkPool->IsEmpty())
    {
    }
  }
  _bRun = false;
  CloseHandle(_hRunEvent);
  if (_pkPool)
  {
    _pkPool->Release();
  }
}

void __cdecl Worker::s_loop(void *pArg)
{
  auto *self = static_cast<Worker *>(pArg);
  WaitForSingleObject(self->_hRunEvent, INFINITE);
  int loopCount = 0;
  while (self->_bRun)
  {
    self->DoWork();
    if (static_cast<unsigned int>(++loopCount) > static_cast<unsigned int>(self->_nLoop))
    {
      Sleep(self->_nTerm);
      loopCount = 0;
    }
  }
  _endthread();
}
