#pragma once

#include "IdaCompat.h"
#include "StorageList.h"
#include "ListHeap.h"

class CPlayer;

class  TimeLimitJade
{
public:
  struct WaitCell
  {
    _STORAGE_LIST::_db_con *_pkItem;
    unsigned __int16 _wStartTime;
    unsigned int _dwUseTime;

    WaitCell();
    WaitCell(_STORAGE_LIST::_db_con *pItem);
    WaitCell(_STORAGE_LIST::_db_con *pItem, unsigned __int16 wStartTime, unsigned int dwUseTime);
    bool operator==(const WaitCell *rhs) const;
    bool operator>(const WaitCell *rhs) const;
  };

  struct UseCell
  {
    _STORAGE_LIST::_db_con *_pkItem;
    unsigned __int16 _wStartTime;
    unsigned int _dwUseTime;

    UseCell();
    UseCell(_STORAGE_LIST::_db_con *pItem);
    UseCell(_STORAGE_LIST::_db_con *pItem, unsigned __int16 wStartTime, unsigned int dwUseTime);
    bool operator==(const UseCell *rhs) const;
    bool operator>(const UseCell *rhs) const;
  };

  explicit TimeLimitJade(CPlayer *p);
  ~TimeLimitJade();

  bool Init();
  bool InitWait();
  bool InitUse();
  void Release();
  bool InsertWaitList(_STORAGE_LIST::_db_con *pkItem);
  bool InsertUseList(_STORAGE_LIST::_db_con *pkItem, unsigned __int16 dwStart, unsigned int dwEnd);
  bool DeleteWaitList(_STORAGE_LIST::_db_con *pkItem);
  bool DeleteUseList(_STORAGE_LIST::_db_con *pkItem, bool bItemDel);
  __int64 CheckStartTime();
  __int64 CheckEndTime();

  CPlayer *_pkOwner = nullptr;
  ListHeap<WaitCell> _heapWaitRow;
  ListHeap<UseCell> _heapUseRow;
};
