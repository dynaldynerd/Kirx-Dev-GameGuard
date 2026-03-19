#pragma once

#include "IdaCompat.h"

/* 1535 */
#pragma pack(push, 1)
struct   _STORAGE_LIST
{
  #pragma pack(push, 1)
  struct   _storage_con
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

    void Init();
    void empty();
    void lock(bool bLock);

    _storage_con()
    {
      std::memset(this, 0, sizeof(_storage_con));
      m_dwLv = 268435455;
    }
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct   _db_con : _storage_con
  {
    _STORAGE_LIST *m_pInList;
    unsigned __int8 m_byStorageIndex;

    void Init();
    void SetSerialNumber(unsigned int dwSN);
    unsigned int GetSerialNumber() const;
    static unsigned int CalcNewSerialNumber();
    _db_con()
    {
      m_pInList = nullptr;
      m_byStorageIndex = static_cast<unsigned __int8>(-1);
    }
  };
  #pragma pack(pop)

  int m_nListNum;
  int m_nUsedNum;
  int m_nListCode;
  _db_con *m_pStorageList;

  _STORAGE_LIST();
  void SetMemory(_db_con *pList, int nListName, int nListNum, int nUsedNum);
  bool EmptyCon(int n);
  void SetAllEmpty();
  void SetLock(int n, bool bLock);
  unsigned int TransInCon(_storage_con *pCon);
  int GetIndexEmptyCon();
  int GetNumUseCon() const;
  int GetNumEmptyCon();
  bool AlterCurDur(int n, int nAlter, unsigned __int64 *pdwLeftDur);
  bool UpdateCurDur(int n, int nUpdate);
  bool SetUseListNum(int nUsedNum);
  int GetIndexFromSerial(unsigned __int16 wSerial);
  _db_con *GetPtrFromSerial(unsigned __int16 wSerial);
  _db_con *GetPtrFromItemCode(char *pwszItemCode);
  bool GradeUp(int n, unsigned int dwUptInfo);
  bool GradeDown(int n, unsigned int dwUptInfo);
  bool SetGrade(int n, unsigned __int8 byLv, unsigned int dwUptInfo);
};
#pragma pack(pop)

_STORAGE_LIST::_db_con *MakeLoot(unsigned __int8 byTableCode, unsigned __int16 wItemIndex);

