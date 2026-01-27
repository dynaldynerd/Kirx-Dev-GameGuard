#include "pch.h"

#include "CMerchant.h"

#include <cstring>

void CMerchant::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pItemStore = nullptr;
  m_dwLastDestroyTime = 0;
  m_byRaceCode = 0;
  std::memset(m_nLeftTicketNum, -1, sizeof(m_nLeftTicketNum));
}

int CMerchant::s_nLiveNum = 0;

bool CMerchant::Create(_npc_create_setdata *pData)
{
    // Simplified Create for now
    this->m_bLive = true;
    this->m_pItemStore = pData->m_pLinkItemStore;
    this->m_dwObjSerial = GetNewMonSerial();
    this->m_byRaceCode = pData->m_byRaceCode;
    this->m_pCurMap = pData->m_pMap;
    this->m_fCurPos[0] = pData->m_fStartPos[0];
    this->m_fCurPos[1] = pData->m_fStartPos[1];
    this->m_fCurPos[2] = pData->m_fStartPos[2];
    
    SendMsg_Create(this);
    ++s_nLiveNum;
    return true;
}

unsigned int CMerchant::GetNewMonSerial()
{
    static unsigned int s_dwSerial = 0;
    return ++s_dwSerial;
}

void CMerchant::SendMsg_Create(CMerchant *pMerchant)
{
    // Stub
}
