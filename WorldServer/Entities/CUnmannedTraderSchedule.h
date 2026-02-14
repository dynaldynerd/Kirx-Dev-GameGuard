#pragma once

#include "IdaCompat.h"

class __cppobj CUnmannedTraderSchedule
{
public:
  enum class STATE : int
  {
    EMPTY = 0,
    REG_WAIT = 1,
    WAIT_CANCEL = 2,
    CANCEL_SUCC_COMPLETE = 3,
    CANCEL_FAIL_COMPLETE = 4,
  };

  CUnmannedTraderSchedule();
  ~CUnmannedTraderSchedule();
  void Clear();
  void Set(
    unsigned __int8 byType,
    unsigned int dwSerial,
    __int64 tEndTime,
    unsigned int dwOwnerSerial,
    unsigned int dwK);
  unsigned __int8 GetType();
  unsigned int GetRegistSerial();
  unsigned int GetOwnerSerial();
  bool IsWait();
  bool IsDone();
  void PushClear(bool bTimeLimit);
  void CompleteClear(unsigned __int8 byDBQueryRet, unsigned __int8 byProcRet);

  STATE m_eState;
  unsigned char m_byType;
  unsigned int m_dwRegistSerial;
  long long m_tEndTime;
  unsigned int m_dwOwnerSerial;
  unsigned char m_byItemTableCode;
  unsigned short m_wItemTableIndex;
};
