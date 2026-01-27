#include "pch.h"

#include "CNetCriticalSection.h"

CNetCriticalSection::CNetCriticalSection()
{
  InitializeCriticalSection(&m_cs);
}

CNetCriticalSection::~CNetCriticalSection()
{
  DeleteCriticalSection(&m_cs);
}

void CNetCriticalSection::Lock()
{
  EnterCriticalSection(&m_cs);
}

void CNetCriticalSection::Unlock()
{
  LeaveCriticalSection(&m_cs);
}

