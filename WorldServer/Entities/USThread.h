#pragma once

#include "IdaCompat.h"

#include <cstdint>
#include <cstring>
#include <windows.h>

namespace US
{
  class CNoneCopyAble
  {
  public:
    CNoneCopyAble();
    ~CNoneCopyAble();

  protected:
    CNoneCopyAble(const CNoneCopyAble &) = delete;
    CNoneCopyAble &operator=(const CNoneCopyAble &) = delete;
  };

  class AbstractThread : public CNoneCopyAble
  {
  public:
    virtual ~AbstractThread();

  protected:
    AbstractThread();
  };

  class AbstractThreadPool : public CNoneCopyAble
  {
  public:
    virtual ~AbstractThreadPool();
    virtual void NotifyTerminate(AbstractThread *thread) = 0;
  };

  template <typename TOwner, typename TThreadPool>
  class ThreadParamInterface
  {
  public:
    ThreadParamInterface();
    TThreadPool *GetMyThreadPool();

    TOwner *m_pOwner;
    TThreadPool *m_pMyThreadPool;
    int (__fastcall *m_pOwnerMemberFunc)(TOwner *self, void *);
    std::uint8_t m_Unknown[16];
  };

  template <typename TParam>
  class CWinThread : public AbstractThread
  {
  public:
    CWinThread();
    virtual ~CWinThread();

    bool Create(TParam *pThreadParam, unsigned int (__stdcall *pDefaultThread)(void *));
    bool IsRunning();
    void Terminate(DWORD dwWaitTime);
    void SetStartEvent();
    virtual __int64 WorkProc();
    virtual void Loop();
    static unsigned int __stdcall WorkerThread(void *pParam);

  protected:
    volatile bool m_bRunning;
    TParam m_ThreadParam;
    HANDLE m_hThread;
    HANDLE m_hStartupEvent;
    HANDLE m_hDestroyEvent;
    DWORD m_dwThreadID;
  };

  template <typename TOwner, typename TThreadPool>
  ThreadParamInterface<TOwner, TThreadPool>::ThreadParamInterface()
    : m_pOwner(nullptr),
      m_pMyThreadPool(nullptr),
      m_pOwnerMemberFunc(nullptr),
      m_Unknown{}
  {
    std::memset(m_Unknown, 0, sizeof(m_Unknown));
    *reinterpret_cast<int *>(m_Unknown) = -1;
  }

  template <typename TOwner, typename TThreadPool>
  TThreadPool *ThreadParamInterface<TOwner, TThreadPool>::GetMyThreadPool()
  {
    return m_pMyThreadPool;
  }

  template <typename TParam>
  CWinThread<TParam>::CWinThread()
    : m_bRunning(false),
      m_ThreadParam(),
      m_hThread(nullptr),
      m_hStartupEvent(nullptr),
      m_hDestroyEvent(nullptr),
      m_dwThreadID(static_cast<DWORD>(-1))
  {
  }

  template <typename TParam>
  CWinThread<TParam>::~CWinThread()
  {
    Terminate(INFINITE);
  }

  template <typename TParam>
  bool CWinThread<TParam>::Create(TParam *pThreadParam, unsigned int (__stdcall *pDefaultThread)(void *))
  {
    m_hStartupEvent = CreateEventA(nullptr, 0, 0, nullptr);
    if (!m_hStartupEvent)
    {
      return false;
    }
    m_hDestroyEvent = CreateEventA(nullptr, 0, 0, nullptr);
    if (!m_hDestroyEvent)
    {
      return false;
    }
    memcpy_s(&m_ThreadParam, sizeof(m_ThreadParam), pThreadParam, sizeof(m_ThreadParam));
    m_hThread = CreateThread(
      nullptr,
      0,
      reinterpret_cast<LPTHREAD_START_ROUTINE>(pDefaultThread),
      this,
      0,
      &m_dwThreadID);
    if (!m_hThread)
    {
      return false;
    }
    WaitForSingleObject(m_hStartupEvent, -1);
    return true;
  }

  template <typename TParam>
  bool CWinThread<TParam>::IsRunning()
  {
    return m_bRunning;
  }

  template <typename TParam>
  void CWinThread<TParam>::Terminate(DWORD dwWaitTime)
  {
    if (m_bRunning)
    {
      if (m_hThread)
      {
        m_bRunning = false;
        SetEvent(m_hDestroyEvent);
        DWORD waitResult = WaitForSingleObject(m_hThread, dwWaitTime);
        if (waitResult == WAIT_TIMEOUT)
        {
          TerminateThread(m_hThread, 0);
        }
      }
    }
    CloseHandle(m_hThread);
    m_hThread = nullptr;
    m_dwThreadID = static_cast<DWORD>(-1);
    CloseHandle(m_hStartupEvent);
    m_hStartupEvent = nullptr;
    CloseHandle(m_hDestroyEvent);
    m_hDestroyEvent = nullptr;
    TParam *threadParam = &m_ThreadParam;
    auto *myThreadPool = threadParam->GetMyThreadPool();
    if (myThreadPool)
    {
      myThreadPool->NotifyTerminate(this);
    }
  }

  template <typename TParam>
  void CWinThread<TParam>::SetStartEvent()
  {
    SetEvent(m_hStartupEvent);
    m_bRunning = true;
  }

  template <typename TParam>
  __int64 CWinThread<TParam>::WorkProc()
  {
    while (IsRunning())
    {
      Loop();
    }
    return 0;
  }

  template <typename TParam>
  void CWinThread<TParam>::Loop()
  {
    // this is not a stub
  }

  template <typename TParam>
  unsigned int __stdcall CWinThread<TParam>::WorkerThread(void *pParam)
  {
    auto *thread = static_cast<CWinThread<TParam> *>(pParam);
    thread->SetStartEvent();
    return static_cast<unsigned int>(thread->WorkProc());
  }
} // namespace US
