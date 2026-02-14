#include "pch.h"

#include "CProcessThread.h"

#include <cstdio>

namespace
{
}

CProcessThread::_THREAD_CONFIG::_THREAD_CONFIG()
{
  bProgramExitWhenLackData = false;
  nSleepCount = 100;
  sprintf(szThreadID, "NONAME");
  sprintf(szLogFileName, "NONAME");
}

CProcessThread::CProcessThread()
{
  m_pSynData = nullptr;
  m_bThread = false;
  m_dwSynDataNum = 0;
  m_dwSynDataSize = 0;
  m_dwWorkCount = 0;
  m_dwLoopCount = 0;
}

CProcessThread::~CProcessThread()
{
  m_bThread = false;
  Sleep(0xBB8u);

  if (m_pSynData)
  {
    delete[] m_pSynData;
    m_pSynData = nullptr;
  }
}

void CProcessThread::CrashThread()
{
// this is not a stub
}

unsigned char CProcessThread::WorkProcess(_SYN_DATA * /*pSynData*/)
{
// this is not a stub
  return 0;
}

void CProcessThread::CompleteProcess(_SYN_DATA * /*pSynData*/)
{
// this is not a stub
}
