#include "pch.h"

#include "CWorldSchedule.h"

#include <cstdio>
#include <cstring>
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

bool CWorldSchedule::Init(CWorldSchedule *thisPtr)
{
  if (thisPtr->m_bOper)
    return false;

  // Logging omitted for brevity unless g_logSchedule is provided
  thisPtr->m_dwLastCheckTime = GetTickCount();

  char pszErrMsg[132]{};
  if (thisPtr->m_tblSch.ReadRecord(".\\Script\\Schedule.dat", 0x5C, pszErrMsg))
  {
    thisPtr->m_nMaxSchNum = thisPtr->m_tblSch.GetRecordNum();
    if (thisPtr->DataCheck())
    {
      thisPtr->m_nCurHour = GetCurrentHour();
      thisPtr->m_nCurMin = GetCurrentMin();
      thisPtr->m_nCurMilSec = 1000 * GetCurrentSec();
      thisPtr->m_nSchCursor = thisPtr->CalcScheduleCursor(thisPtr->m_nCurHour, thisPtr->m_nCurMin);
      
      thisPtr->m_tmrCheck.BeginTimer(1000);
      thisPtr->m_bOper = true;
      return true;
    }
  }
  return false;
}

bool CWorldSchedule::DataCheck()
{
  return true;
}

int CWorldSchedule::CalcScheduleCursor(int nHour, int nMin)
{
  return 0;
}
