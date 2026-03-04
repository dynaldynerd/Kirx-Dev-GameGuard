#pragma once

#include "CRFNewDatabase.h"
#include "LTD.h"

class  CRFDBItemLog : public CRFNewDatabase
{
public:
  explicit CRFDBItemLog(unsigned int dwLocalDate = 0);

  ~CRFDBItemLog() override;

  bool CreateTblLtd(int nKorTime);
  bool CreateTblLtd_ItemInfo(int nKorTime);
  bool CreateTblLtd_Expend(int nKorTime);
  bool insert_ltd(_LTD *pl);
  bool insert_iteminfo(_LTD_ITEMINFO *pi, unsigned __int8 byIndex);
  bool insert_expend(_LTD_EXPEND *pe);
  void SetKorTime(unsigned int dwKorTime);

  bool m_bTblLtd;
  bool m_bTblLtdItemInfo;
  bool m_bTblLtdExpend;
  unsigned int m_dwKorTime;
};
