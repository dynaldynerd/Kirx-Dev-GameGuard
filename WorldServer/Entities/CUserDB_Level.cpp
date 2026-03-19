#include "pch.h"

#include "CUserDB.h"

#include "CLogFile.h"
#include "GlobalObjects.h"
#include "log_sheet_lv.h"

void CUserDB::WriteLog_Level(unsigned __int8 byLv)
{
  if ( !this->m_bField )
  {
    return;
  }

  _log_sheet_lv pQryData{};
  pQryData.dwAvatorSerial = this->m_dwSerial;
  pQryData.byLv = byLv;
  pQryData.dwTotalPlayMin = this->m_AvatorData.dbAvator.m_dwTotalPlayMin;

  int nSize = static_cast<int>(pQryData.size());
  g_Main.PushDQSData(
    this->m_dwAccountSerial,
    &this->m_idWorld,
    7u,
    reinterpret_cast<char *>(&pQryData),
    nSize);
}

bool CUserDB::Update_Level(unsigned __int8 lv, long double exp)
{
  if ( this->m_AvatorData.dbAvator.m_byMaxLevel )
  {
    if ( lv > this->m_AvatorData.dbAvator.m_byMaxLevel && (!this->m_byUserDgr || this->m_byUserDgr == 1) )
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_Level(): pRecv->byLv (%d) > max level (%d) => failed ",
        this->m_aszAvatorName,
        lv,
        this->m_AvatorData.dbAvator.m_byMaxLevel);
      return false;
    }
  }
  else if ( lv > 50u )
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_Level(): pRecv->byLv (%d) => failed ",
      this->m_aszAvatorName,
      lv);
    return false;
  }

  if ( lv > this->m_AvatorData.dbAvator.m_byLevel || this->m_byUserDgr )
  {
    this->m_AvatorData.dbAvator.m_byLevel = lv;
    this->m_AvatorData.dbAvator.m_dExp = exp;
    if ( !this->m_byUserDgr )
    {
      this->WriteLog_Level(lv);
    }
    this->m_bDataUpdate = 1;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_Level(): OLD(%d) >= NEW(%d) => failed ",
    this->m_aszAvatorName,
    this->m_AvatorData.dbAvator.m_byLevel,
    lv);
  return false;
}

bool CUserDB::Update_PlayTime(unsigned int dwTotalTimeMin)
{
  this->m_AvatorData.dbAvator.m_dwTotalPlayMin = dwTotalTimeMin;
  return true;
}

void CUserDB::TotalPlayMinCheck()
{
  if ( this->m_dwSerial != static_cast<unsigned int>(-1) && this->m_tmrCheckPlayMin.CountingTimer() )
  {
    Update_PlayTime(this->m_AvatorData.dbAvator.m_dwTotalPlayMin + 1);
  }
}

void CUserDB::OnLoop_Static()
{
  CUserDB::s_MgrLobbyHistory.OnLoop();
}
