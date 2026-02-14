#pragma once

#include "CRFNewDatabase.h"

class __cppobj CRFDBItemLog : public CRFNewDatabase
{
public:
  explicit CRFDBItemLog(unsigned int dwLocalDate = 0);

  ~CRFDBItemLog() override;

  bool m_bTblLtd;
  bool m_bTblLtdItemInfo;
  bool m_bTblLtdExpend;
  unsigned int m_dwKorTime;
};
