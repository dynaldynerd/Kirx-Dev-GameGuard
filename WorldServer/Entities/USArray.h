#pragma once

#include "IdaCompat.h"

#include <new>

namespace US
{
template <typename T>
class CArray
{
public:
  CArray() : m_bAlloc(false), m_pBuffer(nullptr), m_dwCount(0) {}
  virtual ~CArray() { Free(); }

  void Alloc(unsigned int count)
  {
    if (m_bAlloc || !count)
    {
      return;
    }

    T *buffer = new (std::nothrow) T[count];
    if (!buffer)
    {
      return;
    }

    m_pBuffer = buffer;
    m_dwCount = count;
    m_bAlloc = true;
  }

  void Free()
  {
    if (!m_bAlloc)
    {
      return;
    }

    delete[] m_pBuffer;
    m_pBuffer = nullptr;
    m_dwCount = 0;
    m_bAlloc = false;
  }

  unsigned int GetSize() const { return m_dwCount; }

  T *GetAtPtr(unsigned int index)
  {
    if (!m_pBuffer || index >= m_dwCount)
    {
      return nullptr;
    }
    return &m_pBuffer[index];
  }

  const T *GetAtPtr(unsigned int index) const
  {
    if (!m_pBuffer || index >= m_dwCount)
    {
      return nullptr;
    }
    return &m_pBuffer[index];
  }

  unsigned int GetIndex(const T *ptr) const
  {
    if (!ptr || !m_pBuffer)
    {
      return static_cast<unsigned int>(-1);
    }

    if (ptr < m_pBuffer || ptr >= (m_pBuffer + m_dwCount))
    {
      return static_cast<unsigned int>(-1);
    }

    return static_cast<unsigned int>(ptr - m_pBuffer);
  }

  bool m_bAlloc;
  T *m_pBuffer;
  unsigned int m_dwCount;
};

template <typename T, typename State>
class CArrayEx
{
public:
  struct ORDER_INC
  {
  };

  CArrayEx() = default;
  ~CArrayEx() = default;

  void Alloc(unsigned int count)
  {
    m_DataAr.Alloc(count);
    m_StateAr.Alloc(count);
  }

  unsigned int GetSize() const { return m_DataAr.GetSize(); }

  T *GetAtPtr(unsigned int index) { return m_DataAr.GetAtPtr(index); }
  const T *GetAtPtr(unsigned int index) const { return m_DataAr.GetAtPtr(index); }

  State *GetStateAtPtr(unsigned int index) { return m_StateAr.GetAtPtr(index); }
  const State *GetStateAtPtr(unsigned int index) const { return m_StateAr.GetAtPtr(index); }

  unsigned int GetIndex(const T *ptr) const { return m_DataAr.GetIndex(ptr); }

  template <typename OrderTag>
  static unsigned int SearchSlotIndex(CArrayEx *array, const State *state)
  {
    if (!array || !state)
    {
      return static_cast<unsigned int>(-1);
    }

    const unsigned int size = array->GetSize();
    for (unsigned int index = 0; index < size; ++index)
    {
      const State *cur = array->GetStateAtPtr(index);
      if (cur && (*cur == *state))
      {
        return index;
      }
    }

    return static_cast<unsigned int>(-1);
  }

  CArray<T> m_DataAr;
  CArray<State> m_StateAr;
};
} // namespace US
