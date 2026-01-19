#pragma once

#include "CRFNewDatabase.h"

class __cppobj CRFDBItemLog : public CRFNewDatabase
{
public:
  bool m_bTblLtd;
  bool m_bTblLtdItemInfo;
  bool m_bTblLtdExpend;
  unsigned int m_dwKorTime;
};
