#include "pch.h"

#include "CSyncCS.h"

#include <Windows.h>

void CSyncCS::SetUse(CSyncCS *cs, bool use)
{
  if (cs == nullptr)
  {
    return;
  }
  cs->m_bUse = use;
  if (use)
  {
    InitializeCriticalSection(&cs->m_cs);
  }
}

void CSyncCS::Lock(CSyncCS *cs)
{
  if (cs == nullptr || !cs->m_bUse)
  {
    return;
  }
  EnterCriticalSection(&cs->m_cs);
}

void CSyncCS::Unlock(CSyncCS *cs)
{
  if (cs == nullptr || !cs->m_bUse)
  {
    return;
  }
  LeaveCriticalSection(&cs->m_cs);
}

CSyncCS::~CSyncCS() = default;

