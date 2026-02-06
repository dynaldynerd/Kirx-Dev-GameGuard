#include "pch.h"

#include "USTaskPool.h"

namespace US
{
  CriticalSection::CriticalSection()
  {
    InitializeCriticalSection(&m_CS_);
  }

  CriticalSection::~CriticalSection()
  {
    DeleteCriticalSection(&m_CS_);
  }

  void CriticalSection::Lock()
  {
    EnterCriticalSection(&m_CS_);
  }

  void CriticalSection::UnLock()
  {
    LeaveCriticalSection(&m_CS_);
  }
} // namespace US
