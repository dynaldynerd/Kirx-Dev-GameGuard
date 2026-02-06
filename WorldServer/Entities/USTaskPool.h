#pragma once

#include "IdaCompat.h"

#include <assert.h>
#include <cstring>
#include <functional>
#ifndef _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#endif
#include <hash_map>
#include <vector>
#include <windows.h>

namespace US
{
  class CriticalSection
  {
  public:
    CriticalSection();
    ~CriticalSection();

    void Lock();
    void UnLock();

    CRITICAL_SECTION m_CS_{};
  };

  template <typename Mutex>
  class CScope_Lock
  {
  public:
    explicit CScope_Lock(Mutex *mtx)
      : m_Mutex_(mtx)
    {
      if (m_Mutex_)
      {
        m_Mutex_->Lock();
      }
    }

    ~CScope_Lock()
    {
      if (m_Mutex_)
      {
        m_Mutex_->UnLock();
      }
    }

  private:
    Mutex *m_Mutex_;
  };

  template <typename T, typename Lock, int kTag>
  class CCircularFIFO
  {
  public:
    CCircularFIFO()
      : m_pPool(nullptr),
        m_StartPos(0),
        m_EndPos(0),
        m_Size(0),
        m_AllocSize(0)
    {
    }

    ~CCircularFIFO()
    {
      Free();
    }

    void Alloc(unsigned __int64 tAllocSize)
    {
      CScope_Lock<Lock> lock(&m_CS);
      m_AllocSize = tAllocSize;
      if (m_AllocSize)
      {
        m_pPool = static_cast<T *>(
          operator new[](saturated_mul(static_cast<size_t>(m_AllocSize), sizeof(T))));
      }
      if (!m_pPool)
      {
        _wassert(L"m_pPool", L".\\../Common/USGeneric/CircularFIFO.h", 0x61u);
      }
    }

    void Free()
    {
      CScope_Lock<Lock> lock(&m_CS);
      if (m_pPool)
      {
        operator delete[](m_pPool);
      }
      m_pPool = nullptr;
      m_StartPos = 0;
      m_EndPos = 0;
      m_Size = 0;
      m_AllocSize = 0;
    }

    bool PushBack(const T *rhs)
    {
      CScope_Lock<Lock> lock(&m_CS);
      if (m_Size < m_AllocSize)
      {
        memcpy_s(&m_pPool[m_EndPos], sizeof(T), rhs, sizeof(T));
        ++m_Size;
        if (++m_EndPos == m_AllocSize)
        {
          m_EndPos = 0;
        }
        return true;
      }
      return false;
    }

    bool PopFront(T *pOut)
    {
      CScope_Lock<Lock> lock(&m_CS);
      if (!pOut)
      {
        _wassert(L"pOut", L".\\../Common/USGeneric/CircularFIFO.h", 0x3Bu);
      }
      if (m_Size)
      {
        memcpy_s(pOut, sizeof(T), &m_pPool[m_StartPos], sizeof(T));
        --m_Size;
        if (++m_StartPos == m_AllocSize)
        {
          m_StartPos = 0;
        }
        return true;
      }
      return false;
    }

    void ReAlloc(unsigned __int64 tReAllocSize)
    {
      CScope_Lock<Lock> lock(&m_CS);
      if (tReAllocSize)
      {
        if (tReAllocSize > m_AllocSize)
        {
          if (m_AllocSize)
          {
            CCircularFIFO temp;
            temp.Alloc(tReAllocSize);
            T out{};
            while (PopFront(&out))
            {
              temp.PushBack(&out);
            }
            Free();
            Alloc(tReAllocSize);
            while (temp.PopFront(&out))
            {
              PushBack(&out);
            }
          }
          else
          {
            Alloc(tReAllocSize);
          }
        }
      }
      else
      {
        Free();
      }
    }

    Lock m_CS;
    T *m_pPool;
    unsigned __int64 m_StartPos;
    unsigned __int64 m_EndPos;
    unsigned __int64 m_Size;
    unsigned __int64 m_AllocSize;
  };

  template <typename Task, typename Queue>
  class AbstractTaskPool
  {
  public:
    struct Default_Funtor
    {
      virtual ~Default_Funtor() = default;
      virtual void operator()(Task *task) = 0;
    };

    AbstractTaskPool()
      : m_dwAllocSize(0)
    {
    }

    virtual ~AbstractTaskPool() = default;

    virtual bool ReAllocPool(unsigned int /*dwAllocSize*/)
    {
      return false;
    }

    virtual void FreePool()
    {
    }

    virtual unsigned int GetTaskIndex(Task * /*pTask*/)
    {
      return static_cast<unsigned int>(-1);
    }

    virtual Task *GetAtPtr(unsigned int /*tIndex*/)
    {
      return nullptr;
    }

    Task *PopEmpty()
    {
      unsigned long outIndex = 0;
      if (m_EmptyIndexQueue.PopFront(&outIndex))
      {
        return GetAtPtr(static_cast<unsigned int>(outIndex));
      }
      const unsigned int nextSize = 2 * m_dwAllocSize;
      if (ReAllocPool(nextSize))
      {
        return PopEmpty();
      }
      return nullptr;
    }

    Task *PopProc()
    {
      unsigned long outIndex = static_cast<unsigned long>(-1);
      if (m_UsedIndexQueue.PopFront(&outIndex))
      {
        return GetAtPtr(static_cast<unsigned int>(outIndex));
      }
      return nullptr;
    }

    bool PushEmpty(Task *pTask)
    {
      unsigned long index = static_cast<unsigned long>(GetTaskIndex(pTask));
      return index != static_cast<unsigned long>(-1) && m_EmptyIndexQueue.PushBack(&index);
    }

    bool PushProc(Task *pTask)
    {
      unsigned long index = static_cast<unsigned long>(GetTaskIndex(pTask));
      return index != static_cast<unsigned long>(-1) && m_UsedIndexQueue.PushBack(&index);
    }

    Queue m_EmptyIndexQueue;
    Queue m_UsedIndexQueue;
    Queue m_CompleteIndexQueue;
    unsigned int m_dwAllocSize;
  };

  template <typename Task, typename Queue>
  class CDynamicTaskPool : public AbstractTaskPool<Task, Queue>
  {
  public:
    CDynamicTaskPool()
      : m_pInitFunctor(nullptr)
    {
      m_TaskPool.clear();
    }

    ~CDynamicTaskPool() override
    {
      FreePool();
      if (m_pInitFunctor)
      {
        operator delete(m_pInitFunctor);
      }
    }

    bool ReAllocPool(unsigned int dwAllocSize) override
    {
      CScope_Lock<CriticalSection> lock(&m_CS);
      const unsigned int curSize = static_cast<unsigned int>(m_TaskPool.size());
      if (curSize < dwAllocSize)
      {
        this->m_dwAllocSize = dwAllocSize;
        const unsigned __int64 addCount = dwAllocSize - curSize;
        this->m_EmptyIndexQueue.ReAlloc(this->m_dwAllocSize);
        this->m_UsedIndexQueue.ReAlloc(this->m_dwAllocSize);
        this->m_CompleteIndexQueue.ReAlloc(this->m_dwAllocSize);
        for (unsigned int j = 0; j < addCount; ++j)
        {
          Task *task = static_cast<Task *>(operator new(sizeof(Task)));
          if (task)
          {
            new (task) Task();
          }
          Task *taskPtr = task;
          const unsigned long index = static_cast<unsigned long>(m_TaskPool.size());
          m_TaskPool.push_back(taskPtr);
          this->m_EmptyIndexQueue.PushBack(&index);
          m_PtrHashTable[taskPtr] = index;
          if (m_pInitFunctor)
          {
            Task *stored = m_TaskPool[index];
            (*m_pInitFunctor)(stored);
          }
        }
      }
      return true;
    }

    void FreePool() override
    {
      for (Task *task : m_TaskPool)
      {
        operator delete(task);
      }
      m_TaskPool.clear();
      m_PtrHashTable.clear();
    }

    unsigned int GetTaskIndex(Task *pTask) override
    {
      CScope_Lock<CriticalSection> lock(&m_CS);
      auto it = m_PtrHashTable.find(pTask);
      if (it != m_PtrHashTable.end())
      {
        return it->second;
      }
      return static_cast<unsigned int>(-1);
    }

    Task *GetAtPtr(unsigned int tIndex) override
    {
      CScope_Lock<CriticalSection> lock(&m_CS);
      return m_TaskPool[tIndex];
    }

    std::vector<Task *> m_TaskPool;
    stdext::hash_map<Task *, unsigned long, stdext::hash_compare<Task *, std::less<Task *>>> m_PtrHashTable;
    typename AbstractTaskPool<Task, Queue>::Default_Funtor *m_pInitFunctor;
    CriticalSection m_CS;
  };
} // namespace US
