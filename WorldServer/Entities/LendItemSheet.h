#pragma once

#include "IdaCompat.h"
#include "StorageList.h"
#include "ListHeap.h"

class CPlayer;

class  LendItemSheet
{
public:
  struct Cell
  {
    unsigned __int8 _nStorageCode;
    _STORAGE_LIST::_db_con *_pkItem;

    Cell();
    Cell(unsigned __int8 byStCode, _STORAGE_LIST::_db_con *pItem);
    bool operator==(const Cell *rhs) const;
    bool operator>(const Cell *rhs) const;
  };

  explicit LendItemSheet(CPlayer *p);
  ~LendItemSheet();

  bool Initialzie();
  bool InsertLink(unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem);
  bool DeleteLink(unsigned __int8 byStorageCode, _STORAGE_LIST::_db_con *pkItem);
  void Release();
  int CheckTime();

  CPlayer *_pkOwner = nullptr;
  ListHeap<Cell> _heapFixRow;
};
