#include "pch.h"

#include "CMyCriticalSection.h"

CMyCriticalSection::CMyCriticalSection()
{
  InitializeCriticalSection(&m_cs);
}

void CMyCriticalSection::Lock()
{
  EnterCriticalSection(&m_cs);
}

void CMyCriticalSection::Unlock()
{
  LeaveCriticalSection(&m_cs);
}

CMyCriticalSection::~CMyCriticalSection()
{
  DeleteCriticalSection(&m_cs);
}
