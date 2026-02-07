#include "pch.h"

#include "CWorldSchedule.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <mmsystem.h>

#include "CHolyStoneSystem.h"
#include "CLogFile.h"
#include "CTransportShip.h"
#include "GlobalObjects.h"
#include "WorldSchedule_fld.h"
#include "WorldServerUtil.h"

namespace
{
  const char *kSchedulePassMinFmt = "pass_min:%u";
  const char *kScheduleCursorFmt = "cursor:%u hour:%u min:%u msec:%u time:%s";
  const char *kScheduleEventFmt = "code:%s event:%u info1:%u info2:%u";
}

bool CWorldSchedule::Init()
{
  if (this->m_bOper)
    return false;

  if (!g_logSchedule.m_bInit)
  {
    char logPath[128]{};
    const unsigned int now = GetKorLocalTime();
    sprintf_s(logPath, "..\\ZoneServerLog\\ServiceLog\\Schedule%d.log", now);
    g_logSchedule.SetWriteLogFile(logPath, 1, 0, 1, 1);
  }

  this->m_dwLastCheckTime = timeGetTime();

  char pszErrMsg[132]{};
  if (this->m_tblSch.ReadRecord(".\\script\\Schedule.dat", 0x5C, pszErrMsg))
  {
    this->m_nMaxSchNum = this->m_tblSch.GetRecordNum();
    if (this->DataCheck())
    {
      this->m_nCurHour = GetCurrentHour();
      this->m_nCurMin = GetCurrentMin();
      this->m_nCurMilSec = 1000 * GetCurrentSec();
      this->m_nSchCursor = this->CalcScheduleCursor(this->m_nCurHour, this->m_nCurMin);

      int nextCursor = this->m_nSchCursor + 1;
      if (nextCursor >= this->m_nMaxSchNum)
        nextCursor = 0;

      auto *current = reinterpret_cast<_WorldSchedule_fld *>(this->m_tblSch.GetRecord(this->m_nSchCursor));
      auto *next = reinterpret_cast<_WorldSchedule_fld *>(this->m_tblSch.GetRecord(nextCursor));

      int nPassMin = 0;
      if (next->m_nHour < current->m_nHour)
      {
        const int hourGap = (this->m_nCurHour < current->m_nHour)
          ? (this->m_nCurHour + 24 - current->m_nHour)
          : (this->m_nCurHour - current->m_nHour);
        nPassMin = this->m_nCurMin - current->m_nMin + 60 * hourGap;
      }
      else
      {
        nPassMin = this->m_nCurMin - current->m_nMin + 60 * (this->m_nCurHour - current->m_nHour);
      }

      g_logSchedule.Write(kSchedulePassMinFmt, static_cast<unsigned int>(nPassMin));
      char timeBuffer[144]{};
      _strtime(timeBuffer);
      g_logSchedule.Write(
        kScheduleCursorFmt,
        static_cast<unsigned int>(this->m_nSchCursor),
        static_cast<unsigned int>(this->m_nCurHour),
        static_cast<unsigned int>(this->m_nCurMin),
        static_cast<unsigned int>(this->m_nCurMilSec),
        timeBuffer);

      if (!current->m_nEventCode)
        this->ChangeSchCursor(current, nPassMin);

      g_logSchedule.Write(
        kScheduleEventFmt,
        current->m_strCode,
        static_cast<unsigned int>(current->m_nEventCode),
        static_cast<unsigned int>(current->m_nEventInfo1),
        static_cast<unsigned int>(current->m_nEventInfo2));

      this->m_tmrCheck.BeginTimer(1000);
      this->m_bOper = true;
      return true;
    }
  }
  else
  {
    MyMessageBox("CWorldSchedule Error", pszErrMsg);
  }

  return false;
}

bool CWorldSchedule::DataCheck()
{
  int prevAnchor = -1;
  for (int j = 0; j < 2 * this->m_nMaxSchNum; ++j)
  {
    auto *record = reinterpret_cast<_WorldSchedule_fld *>(this->m_tblSch.GetRecord(j % this->m_nMaxSchNum));
    if (record->m_nEventCode)
    {
      if (record->m_nEventCode == 1)
      {
        if (record->m_nEventInfo1 >= 2)
        {
          MyMessageBox("CWorldSchedule Error", "%d rec: event_code_holy : pFld->m_nEventInfo1", j);
          return false;
        }
        if (record->m_nEventInfo2 < 1 || record->m_nEventInfo2 > 3)
        {
          MyMessageBox("CWorldSchedule Error", "%d rec: event_code_holy : pFld->m_nEventInfo2", j);
          return false;
        }
      }
    }
    else
    {
      if (prevAnchor != -1)
      {
        if (!prevAnchor && record->m_nEventInfo2 != 1)
        {
          MyMessageBox("CWorldSchedule Error", "%d rec: Anchor Balance Check Fail", j);
          return false;
        }
        if (prevAnchor == 1 && record->m_nEventInfo2)
        {
          MyMessageBox("CWorldSchedule Error", "%d rec: Anchor Balance Check Fail", j);
          return false;
        }
      }
      prevAnchor = record->m_nEventInfo2;
    }
  }
  return true;
}

int CWorldSchedule::CalcScheduleCursor(int nHour, int nMin)
{
  const int currentMin = nMin + 60 * nHour;
  const int recordNum = this->m_tblSch.GetRecordNum();
  for (int n = 0; n < recordNum; ++n)
  {
    int next = n + 1;
    if (next >= recordNum)
      next = 0;

    auto *current = reinterpret_cast<_WorldSchedule_fld *>(this->m_tblSch.GetRecord(n));
    auto *nextRec = reinterpret_cast<_WorldSchedule_fld *>(this->m_tblSch.GetRecord(next));

    const int startMin = current->m_nMin + 60 * current->m_nHour;
    const int endMin = nextRec->m_nMin + 60 * nextRec->m_nHour;

    if (startMin <= endMin)
    {
      if (currentMin >= startMin && currentMin <= endMin)
        return n;
    }
    else if (currentMin >= startMin)
    {
      return n;
    }
  }

  return -1;
}

void CWorldSchedule::ChangeSchCursor(_WorldSchedule_fld *pFld, int nPassMin)
{
  if (pFld->m_nEventCode)
  {
    if (pFld->m_nEventCode == 1 && pFld->m_nEventInfo1 != -1)
      g_HolySys.AlterSchedule(
        static_cast<unsigned __int8>(pFld->m_nEventInfo1),
        static_cast<unsigned __int8>(pFld->m_nEventInfo2));
  }
  else
  {
    const bool anchor = pFld->m_nEventInfo2 != 0;
    for (int j = 0; j < 3; ++j)
      g_TransportShip[j].AlterState(anchor, static_cast<unsigned __int8>(pFld->m_nEventInfo1), nPassMin, pFld->m_nEventInfo3);
  }
}
