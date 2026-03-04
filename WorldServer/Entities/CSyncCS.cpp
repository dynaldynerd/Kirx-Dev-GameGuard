#include "pch.h"

#include "CSyncCS.h"

#include <Windows.h>

CSyncCS::CSyncCS()
  : m_bUse(false)
{
}

bool CSyncCS::IsUse()
{
  return m_bUse;
}

void CSyncCS::SetUse(bool use)
{
  if (this == nullptr)
  {
    return;
  }
  this->m_bUse = use;
  if (use)
  {
    InitializeCriticalSection(&this->m_cs);
  }
}

void CSyncCS::Lock()
{
  if (this == nullptr || !this->m_bUse)
  {
    return;
  }
  EnterCriticalSection(&this->m_cs);
}

void CSyncCS::Unlock()
{
  if (this == nullptr || !this->m_bUse)
  {
    return;
  }
  LeaveCriticalSection(&this->m_cs);
}

CSyncCS::~CSyncCS() = default;

