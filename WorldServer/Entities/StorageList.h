#pragma once

#include "IdaCompat.h"

/* 1535 */
struct __cppobj __unaligned __declspec(align(4)) _STORAGE_LIST
{
  struct __cppobj __unaligned __declspec(align(1)) _storage_con
  {
    unsigned __int8 m_bLoad;
    unsigned __int8 m_byTableCode;
    unsigned __int8 m_byClientIndex;
    unsigned __int16 m_wItemIndex;
    unsigned __int64 m_dwDur;
    unsigned int m_dwLv;
    unsigned __int16 m_wSerial;
    bool m_bLock;
    unsigned int m_dwETSerialNumber;
    unsigned __int64 m_lnUID;
    unsigned __int8 m_byCsMethod;
    unsigned int m_dwT;
    unsigned int m_dwLendRegdTime;

    void empty();
    void lock(bool bLock);

    _storage_con()
    {
      memset_0(this, 0, sizeof(_storage_con));
      m_dwLv = 0xFFFFFFF;
    }
  };

  struct __cppobj __unaligned __declspec(align(2)) _db_con : _storage_con
  {
    _STORAGE_LIST *m_pInList;
    unsigned __int8 m_byStorageIndex;

    _db_con()
    {
      m_pInList = nullptr;
      m_byStorageIndex = static_cast<unsigned __int8>(-1);
    }
  };

  int m_nListNum;
  int m_nUsedNum;
  int m_nListCode;
  _db_con *m_pStorageList;

  bool EmptyCon(int n);
  void SetLock(int n, bool bLock);
  int TransInCon(_storage_con *pCon);
  int GetIndexEmptyCon();
  int GetNumEmptyCon();
};
