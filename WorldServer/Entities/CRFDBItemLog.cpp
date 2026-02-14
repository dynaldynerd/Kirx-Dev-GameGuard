#include "pch.h"

#include "CRFDBItemLog.h"

CRFDBItemLog::CRFDBItemLog(unsigned int dwLocalDate)
  : m_bTblLtd(false), m_bTblLtdItemInfo(false), m_bTblLtdExpend(false), m_dwKorTime(dwLocalDate)
{
}

CRFDBItemLog::~CRFDBItemLog() = default;
