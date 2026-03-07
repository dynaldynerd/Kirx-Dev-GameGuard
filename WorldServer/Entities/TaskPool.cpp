#include "pch.h"

#include "TaskPool.h"

#include <limits>
#include <new>

namespace
{
  bool is_mul_ok(unsigned __int64 count, unsigned __int64 size)
  {
    if (count == 0 || size == 0)
    {
      return true;
    }
    return count <= (std::numeric_limits<unsigned __int64>::max)() / size;
  }
}

TaskPool::TaskPool()
  = default;

__int64 TaskPool::Initialize(int nTskMaxNum, int nMaxTskSize)
{
  _nMaxTskNum = nTskMaxNum;
  if (!_create_task(nMaxTskSize))
  {
    return 4294967292;
  }
  if (!_init_index_lists())
  {
    return 4294967290;
  }
  _bInit = true;
  return 0;
}

void TaskPool::Release()
{
  if (_pTsks)
  {
    delete[] _pTsks;
    _pTsks = nullptr;
  }
}

bool TaskPool::IsEmpty()
{
  return !_listRegedIdx.size() && !_listCompleteIdx.size();
}

Task *TaskPool::PopWaitTsk(unsigned int *nIdx)
{
  if (!_listRegedIdx.size())
  {
    return nullptr;
  }
  if (_listRegedIdx.PopNode_Front(nIdx))
  {
    return &_pTsks[static_cast<unsigned __int64>(*nIdx)];
  }
  return nullptr;
}

Task *TaskPool::PopCompleteTsk(unsigned int *nIdx)
{
  if (!_listCompleteIdx.size())
  {
    return nullptr;
  }
  if (_listCompleteIdx.PopNode_Front(nIdx))
  {
    return &_pTsks[static_cast<unsigned __int64>(*nIdx)];
  }
  return nullptr;
}

__int64 TaskPool::PushWaitTsk(int nTaskCode, unsigned __int8 *p, unsigned __int64 size)
{
  unsigned int outIndex = 0;
  if (!_listEmptyIdx.PopNode_Front(&outIndex))
  {
    return -2;
  }
  _pTsks[static_cast<unsigned __int64>(outIndex)].SetTask(nTaskCode, p, size);
  if (_listRegedIdx.PushNode_Back(outIndex))
  {
    return 0;
  }
  _listEmptyIdx.PushNode_Front(outIndex);
  return 4294967293;
}

__int64 TaskPool::PushCompleteTsk(unsigned int nIdx)
{
  if (_listCompleteIdx.PushNode_Back(nIdx))
  {
    return 0;
  }
  return 4294967293;
}

__int64 TaskPool::PushEmptyTsk(unsigned int nIdx)
{
  if (_listEmptyIdx.PushNode_Back(nIdx))
  {
    return 0;
  }
  return 4294967293;
}

bool TaskPool::_create_task(int nMaxTskSize)
{
  const unsigned __int64 count = static_cast<unsigned __int64>(_nMaxTskNum);
  if (!is_mul_ok(count, sizeof(Task)))
  {
    return false;
  }
  if (!is_mul_ok(sizeof(Task) * count, 1) || (sizeof(Task) * count) > (std::numeric_limits<unsigned __int64>::max)() - 8)
  {
    return false;
  }

  _pTsks = new (std::nothrow) Task[_nMaxTskNum];
  if (!_pTsks)
  {
    return false;
  }

  for (int j = 0; j < _nMaxTskNum; ++j)
  {
    if (!_pTsks[static_cast<unsigned __int64>(j)].Initialize(nMaxTskSize))
    {
      delete[] _pTsks;
      _pTsks = nullptr;
      return false;
    }
  }

  return true;
}

bool TaskPool::_init_index_lists()
{
  if (!_listEmptyIdx.SetList(_nMaxTskNum))
  {
    return false;
  }
  if (!_listRegedIdx.SetList(_nMaxTskNum))
  {
    return false;
  }
  if (!_listCompleteIdx.SetList(_nMaxTskNum))
  {
    return false;
  }
  for (unsigned int dwIndex = 0; dwIndex < static_cast<unsigned int>(_nMaxTskNum); ++dwIndex)
  {
    if (!_listEmptyIdx.PushNode_Back(dwIndex))
    {
      return false;
    }
  }
  return true;
}
