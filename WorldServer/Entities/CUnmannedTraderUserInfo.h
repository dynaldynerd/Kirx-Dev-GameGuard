#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderRegistItemInfo.h"
#include "CUnmannedTraderRequestLimiter.h"
#include <vector>

class __cppobj CUnmannedTraderUserInfo
{
public:
  enum class LOG_IN_STATE : int
  {
    UTUI_EMPTY = 0,
    UTUI_LOGIN = 1,
    UTUI_NONE = -1,
  };

  CUnmannedTraderUserInfo();

  bool Init(unsigned short wInx);
  void Clear();
  void ClearLoadItemInfo();
  bool IsNull();

  static CUnmannedTraderUserInfo ms_kNull;

  LOG_IN_STATE m_eState;
  unsigned short m_wInx;
  unsigned int m_dwUserSerial;
  unsigned char m_byRegistCnt;
  unsigned char m_byMaxRegistCnt;
  CUnmannedTraderRequestLimiter m_kRequestState;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecRegistItemInfo;
  std::vector<CUnmannedTraderRegistItemInfo> m_vecLoadItemInfo;
};
