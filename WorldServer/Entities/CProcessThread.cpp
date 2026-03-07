#include "pch.h"

#include "CProcessThread.h"

#include <cstdio>
#include <cstring>
#include <process.h>

CProcessThread::_THREAD_CONFIG::_THREAD_CONFIG()
{
  bProgramExitWhenLackData = false;
  nSleepCount = 100;
  sprintf(szThreadID, "NONAME");
  sprintf(szLogFileName, "NONAME");
}

CProcessThread::_SYN_HEADER::_SYN_HEADER()
{
  m_byResult = 0;
}

CProcessThread::_SYN_DATA::_SYN_DATA()
  : _SYN_HEADER(),
    m_bUse(false),
    m_bProcess(false),
    m_psData(nullptr)
{
}

CProcessThread::_SYN_DATA::~_SYN_DATA()
{
  if (m_psData)
  {
    delete[] m_psData;
  }
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
    Sleep(3000u);

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

char CProcessThread::RegisterSyncData(int /*nCaseCode*/, unsigned int dwDataSize)
{
  if (m_dwSynDataSize <= dwDataSize)
  {
    m_dwSynDataSize = dwDataSize;
  }
  return 1;
}

CProcessThread::_SYN_DATA *CProcessThread::PushData(_SYN_HEADER *pHeader, char *pQryData, unsigned int nQrySize)
{
  if (nQrySize > m_dwSynDataSize)
  {
    m_logSystem.Write(
      "%d : if(nQrySize(%d) > m_dwSynDataSize(%d))",
      pHeader->m_byQryCase,
      nQrySize,
      m_dwSynDataSize);
    return nullptr;
  }

  unsigned int outIndex[5]{};
  if (!m_listDataEmpty.PopNode_Front(outIndex))
  {
    m_logSystem.Write("%d : m_listDataEmpty.PopNode_Front() => failed ", pHeader->m_byQryCase);
    return nullptr;
  }

  _SYN_DATA *synData = &m_pSynData[outIndex[0]];
  memcpy_0(synData, pHeader, 12u);
  if (pQryData)
  {
    memcpy_0(synData->m_psData, pQryData, static_cast<int>(nQrySize));
  }

  if (!m_listData.PushNode_Back(outIndex[0]))
  {
    m_logSystem.Write("%d : m_listData.PushNode_Back() => failed ", pHeader->m_byQryCase);
    return nullptr;
  }

  synData->m_bUse = true;
  synData->m_bProcess = false;
  return synData;
}

void CProcessThread::ProcessOnThread()
{
  m_FrameRate.CalcSpeedPerFrame();

  unsigned int outIndex[5]{};
  while (m_listData.PopNode_Front(outIndex))
  {
    _SYN_DATA *synData = &m_pSynData[outIndex[0]];
    synData->m_byResult = WorkProcess(synData);
    synData->m_bProcess = true;
    m_listDataComplete.PushNode_Back(outIndex[0]);
    ++m_dwWorkCount;
  }

  ++m_dwLoopCount;
}

void CProcessThread::CompleteOnLoop()
{
  unsigned int outIndex[5]{};
  while (m_listDataComplete.PopNode_Front(outIndex))
  {
    _SYN_DATA *synData = &m_pSynData[outIndex[0]];
    CompleteProcess(synData);
    synData->m_bUse = false;
    synData->m_bProcess = false;
    m_listDataEmpty.PushNode_Back(outIndex[0]);
  }
}

void CProcessThread::RunOnLoop()
{
  CompleteOnLoop();
}

void CProcessThread::RunOnThread()
{
  m_FrameRate.CalcSpeedPerFrame();
  ProcessOnThread();
}

char CProcessThread::StartThread(_THREAD_CONFIG *pConfig, unsigned int dwSynDataNum)
{
  m_dwSynDataNum = dwSynDataNum;
  memcpy_0(&m_Config, pConfig, sizeof(m_Config));
  m_logSystem.SetWriteLogFile(pConfig->szLogFileName, 1, false, true, true);

  m_pSynData = new _SYN_DATA[dwSynDataNum];
  if (m_dwSynDataSize)
  {
    for (unsigned int index = 0; index < dwSynDataNum; ++index)
    {
      m_pSynData[index].m_psData = new char[m_dwSynDataSize];
    }
  }

  m_listData.SetList(dwSynDataNum);
  m_listDataComplete.SetList(dwSynDataNum);
  m_listDataEmpty.SetList(dwSynDataNum);
  for (unsigned int index = 0; index < dwSynDataNum; ++index)
  {
    m_listDataEmpty.PushNode_Back(index);
  }

  m_bThread = true;
  _beginthread((_beginthread_proc_type)CProcessThread::Thread, 0, this);
  return 1;
}

void CProcessThread::Thread(void *pv)
{
  CProcessThread *thread = static_cast<CProcessThread *>(pv);
  int loopCount = 0;

  while (thread->m_bThread)
  {
    thread->RunOnThread();
    const int prevLoopCount = loopCount;
    ++loopCount;
    if (prevLoopCount > thread->m_Config.nSleepCount)
    {
      Sleep(1u);
    }
  }

  _endthreadex(0);
}
