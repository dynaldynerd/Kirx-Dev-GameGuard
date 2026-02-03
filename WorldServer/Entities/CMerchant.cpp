#include "pch.h"

#include "CMerchant.h"

#include <cstring>

bool CMerchant::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pItemStore = nullptr;
  m_dwLastDestroyTime = 0;
  m_byRaceCode = 0;
  std::memset(m_nLeftTicketNum, -1, sizeof(m_nLeftTicketNum));
  return true;
}

int CMerchant::s_nLiveNum = 0;
unsigned int CMerchant::s_dwSerialCnt = 0;

bool CMerchant::Create(_npc_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return false;
  }
  m_pItemStore = pData->m_pLinkItemStore;
  m_dwObjSerial = GetNewMonSerial();
  m_byRaceCode = pData->m_byRaceCode;
  SendMsg_Create();
  ++s_nLiveNum;
  return true;
}

unsigned int CMerchant::GetNewMonSerial()
{
  return s_dwSerialCnt++;
}

void CMerchant::SendMsg_Create()
{
  unsigned __int16 wIndex = m_ObjID.m_wIndex;
  unsigned __int8 pbyType[28];
  __int16 pShort[18];
  unsigned __int8 msg[14];

  *reinterpret_cast<unsigned __int16 *>(&msg[0]) = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  *reinterpret_cast<unsigned __int16 *>(&msg[2]) = wIndex;
  *reinterpret_cast<unsigned int *>(&msg[4]) = m_dwObjSerial;
  FloatToShort(m_fCurPos, pShort, 3);
  memcpy_0(&msg[8], pShort, sizeof(__int16) * 3);

  pbyType[0] = 3;
  pbyType[1] = 17;
  CircleReport(pbyType, reinterpret_cast<char *>(msg), 14, false);
}
