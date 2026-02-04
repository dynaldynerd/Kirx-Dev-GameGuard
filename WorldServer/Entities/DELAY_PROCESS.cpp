#include "pch.h"

#include "DELAY_PROCESS.h"

#include <cstring>
#include <mmsystem.h>

_DELAY_PROCESS::_DELAY_PROCESS()
  : m_dwObjectNum(0),
    m_pdwPushTime(nullptr),
    m_pdwPushSerial(nullptr),
    m_list(),
    m_dwTerm(0)
{
}

_DELAY_PROCESS::_DELAY_PROCESS(unsigned int dwObjectNum, unsigned int dwTerm)
  : _DELAY_PROCESS()
{
  Init(dwObjectNum, dwTerm);
}

_DELAY_PROCESS::~_DELAY_PROCESS()
{
  if (m_pdwPushTime)
  {
    delete[] m_pdwPushTime;
    m_pdwPushTime = nullptr;
  }
  if (m_pdwPushSerial)
  {
    delete[] m_pdwPushSerial;
    m_pdwPushSerial = nullptr;
  }
}

bool _DELAY_PROCESS::Init(unsigned int dwObjectNum, unsigned int dwTerm)
{
  if (m_pdwPushTime || dwObjectNum == 0)
  {
    return false;
  }

  m_dwObjectNum = dwObjectNum;
  m_dwTerm = static_cast<int>(dwTerm);
  m_pdwPushTime = new unsigned int[dwObjectNum];
  m_pdwPushSerial = new unsigned int[dwObjectNum];
  CNetIndexList::SetList(&m_list, dwObjectNum);
  std::memset(m_pdwPushSerial, 0xFF, sizeof(unsigned int) * dwObjectNum);
  return true;
}

bool _DELAY_PROCESS::Push(unsigned int dwIndex, unsigned int dwSerial)
{
  if (!m_pdwPushTime)
  {
    return false;
  }

  if (m_pdwPushSerial[dwIndex] != static_cast<unsigned int>(-1))
  {
    return false;
  }

  m_pdwPushTime[dwIndex] = timeGetTime();
  m_pdwPushSerial[dwIndex] = dwSerial;
  return CNetIndexList::PushNode_Back(&m_list, dwIndex);
}

void _DELAY_PROCESS::Delete(unsigned int dwIndex, unsigned int dwSerial)
{
  if (m_pdwPushSerial[dwIndex] == dwSerial && m_pdwPushSerial[dwIndex] != static_cast<unsigned int>(-1))
  {
    if (CNetIndexList::FindNode(&m_list, dwIndex))
    {
      m_pdwPushSerial[dwIndex] = static_cast<unsigned int>(-1);
    }
  }
}

void _DELAY_PROCESS::CheckOnLoop()
{
  if (!m_pdwPushTime)
  {
    return;
  }

  const unsigned int now = timeGetTime();
  unsigned int outIndex[4] = {};
  while (CNetIndexList::CopyFront(&m_list, outIndex))
  {
    const unsigned int index = outIndex[0];
    const int elapsed = static_cast<int>(now - m_pdwPushTime[index]);
    if (elapsed <= m_dwTerm)
    {
      if (elapsed < 0)
      {
        m_pdwPushTime[index] = 0;
      }
      return;
    }

    Process(index, m_pdwPushSerial[index]);
    m_pdwPushSerial[index] = static_cast<unsigned int>(-1);
    CNetIndexList::PopNode_Front(&m_list, outIndex);
  }
}

void _DELAY_PROCESS::Process(unsigned int /*dwIndex*/, unsigned int /*dwSerial*/)
{
}
