#include "pch.h"

#include "Task.h"
#ifdef SHIPDEBUG
#include "RuntimeAssert.h"
#endif

#include <crtdbg.h>
#include <intrin.h>

Task::Task()
{
  _bFill = false;
  _tmReg = MyTimer::TIME();
  _pBuf = nullptr;
  _nBufSize = 0;
  _nMaxBufSize = 0;
}

Task::~Task()
{
  // this is not a stub
}

bool Task::Initialize(unsigned __int64 nMaxBufSize)
{
  _nMaxBufSize = nMaxBufSize;
  _pBuf = static_cast<unsigned __int8 *>(operator new[](_nMaxBufSize));
  return _pBuf != nullptr;
}

void Task::SetTask(int nTaskCode, const unsigned __int8 *p, unsigned __int64 size)
{
  _bFill = true;
  _nTaskCode = nTaskCode;
  std::memcpy(&_tmReg, MyTimer::GetTime(), sizeof(_tmReg));
#ifdef SHIPDEBUG
  if (size >= _nMaxBufSize
      && RuntimeAssert::ShouldBreak(
        L"size < _nMaxBufSize",
        L"g:\\00_zoneserver_source\\03_temp_source\\2009_05_13_source_oversea\\zoneserver\\gamemain\\partiallypaid\\Task.h",
        51))
  {
    __debugbreak();
    return;
  }
#else
  if (size >= _nMaxBufSize
      && _CrtDbgReportW(
           2,
           L"g:\\00_zoneserver_source\\03_temp_source\\2009_05_13_source_oversea\\zoneserver\\gamemain\\partiallypaid\\Task.h",
           51,
           nullptr,
           nullptr)
           == 1)
  {
    __debugbreak();
  }
#endif
  _nBufSize = size;
  std::memcpy(_pBuf, p, _nBufSize);
}

unsigned __int8 *Task::GetTaskBuf()
{
  return _pBuf;
}

int Task::GetTaskCode()
{
  return _nTaskCode;
}

void Task::SetRetCode(int nCode)
{
  _nRetCode = nCode;
}

int Task::GetRetCode()
{
  return _nRetCode;
}
