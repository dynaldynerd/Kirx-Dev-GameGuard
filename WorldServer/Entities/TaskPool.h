#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"
#include "Task.h"

class  TaskPool
{
public:
  TaskPool();

  __int64 Initialize(int nTskMaxNum, int nMaxTskSize);
  void Release();
  bool IsEmpty();
  Task *PopWaitTsk(unsigned int *nIdx);
  Task *PopCompleteTsk(unsigned int *nIdx);
  __int64 PushWaitTsk(int nTaskCode, unsigned __int8 *p, unsigned __int64 size);
  __int64 PushCompleteTsk(unsigned int nIdx);
  __int64 PushEmptyTsk(unsigned int nIdx);

  bool _bInit;
  CNetIndexList _listEmptyIdx;
  CNetIndexList _listRegedIdx;
  CNetIndexList _listCompleteIdx;
  int _nMaxTskNum;
  Task *_pTsks;

private:
  bool _create_task(int nMaxTskSize);
  bool _init_index_lists();
};
