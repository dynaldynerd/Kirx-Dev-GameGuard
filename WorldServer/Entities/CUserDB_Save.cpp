#include "pch.h"

#include "WorldServerUtil.h"

#include "CUserDB.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "RFEvent_ClassRefine.h"
#include "qry_case_contsave.h"

#include <mmsystem.h>

bool CUserDB::UpdateContUserSave(bool bDirect)
{
  if ( this->m_bDBWaitState )
    return false;

  DWORD timeNow = timeGetTime();
  if ( !bDirect )
  {
    if ( timeNow - this->m_dwTermContSaveTime < 300000 )
      return false;
    this->m_dwTermContSaveTime = timeNow;
    if ( !this->m_bDataUpdate && timeNow - this->m_dwLastContSaveTime < 600000 )
      return false;
  }

  _qry_case_contsave pQryData{};
  if ( this->m_AvatorData.m_byHSKTime <= 2u && this->m_AvatorData.m_byCristalBattleDBInfo == 1 )
  {
    this->m_AvatorData.m_byCristalBattleDBInfo = 3;
    this->m_AvatorData.m_byHSKTime = static_cast<unsigned __int8>(-1);
  }

  pQryData.dwAvatorSerial = this->m_dwSerial;
  std::memcpy(&pQryData.NewData, &this->m_AvatorData, sizeof(pQryData.NewData));
  std::memcpy(&pQryData.OldData, &this->m_AvatorData_bk, sizeof(pQryData.OldData));
  pQryData.bUpdateRefineCnt = 0;

  unsigned int wIndex = this->m_idWorld.wIndex;
  if ( g_Main.m_pRFEvent_ClassRefine->IsDbUpdate(wIndex) )
  {
    _event_participant_classrefine *state =
      g_Main.m_pRFEvent_ClassRefine->GetPlayerState(wIndex, this->m_idWorld.dwSerial);
    if ( state )
    {
      pQryData.bUpdateRefineCnt = 1;
      pQryData.byRefinedCnt = state->nCurRefineCnt;
      pQryData.dwRefineDate = state->dwRefineDate;
    }
  }

  int nSize = static_cast<int>(pQryData.size());
  _DB_QRY_SYN_DATA *pushData = g_Main.PushDQSData(
    this->m_dwAccountSerial,
    &this->m_idWorld,
    12,
    reinterpret_cast<char *>(&pQryData),
    nSize);

  if ( pushData )
  {
    this->m_bDBWaitState = 1;
    this->m_pDBPushData = pushData;
    this->m_AvatorData.PostUpdateInit();
    this->m_bDataUpdate = 0;
    this->m_dwLastContSaveTime = timeNow;
    return true;
  }

  return false;
}

bool CUserDB::Setting_Class(char *pszClassCode)
{
  if (!g_Main.m_tblClass.GetRecord(pszClassCode))
  {
    return false;
  }

  std::strcpy(this->m_AvatorData.dbAvator.m_szClassCode, pszClassCode);
  this->m_bDataUpdate = 1;
  return true;
}
