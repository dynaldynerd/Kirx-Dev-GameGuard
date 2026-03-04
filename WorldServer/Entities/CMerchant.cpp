#include "pch.h"

#include "WorldServerUtil.h"

#include "CMerchant.h"

#include <cstring>
#include <mmsystem.h>

#include "CMapData.h"
#include "CNetProcess.h"
#include "CTransportShip.h"
#include "CItemStore.h"
#include "GlobalObjects.h"
#include "pnt_rect.h"
#include "StoreList_fld.h"
#include "npc_fld.h"

CMerchant::CMerchant()
{
}

CMerchant::~CMerchant()
{
}

__int64 CMerchant::GetFireTol()
{
  const _npc_fld *recordSet = reinterpret_cast<const _npc_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(static_cast<int>(recordSet->m_fFireTol));
}

char *CMerchant::GetObjName()
{
  static char objectName[128];
  std::snprintf(
    objectName,
    sizeof(objectName),
    "[NPC][%d] >> %s (pos: %s {%d, %d, %d})",
    static_cast<int>(GetObjRace()),
    reinterpret_cast<const _npc_fld *>(m_pRecordSet)->m_strName,
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return objectName;
}

__int64 CMerchant::GetObjRace()
{
  return m_byRaceCode;
}

_dummy_position *CMerchant::GetStoreDummyName()
{
  if (m_pItemStore && m_pItemStore->m_pDum && m_pItemStore->m_pDum->m_pDumPos)
  {
    return m_pItemStore->m_pDum->m_pDumPos;
  }
  return nullptr;
}

__int64 CMerchant::GetSoilTol()
{
  const _npc_fld *recordSet = reinterpret_cast<const _npc_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(static_cast<int>(recordSet->m_fSoilTol));
}

__int64 CMerchant::GetWaterTol()
{
  const _npc_fld *recordSet = reinterpret_cast<const _npc_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(static_cast<int>(recordSet->m_fWaterTol));
}

__int64 CMerchant::GetWindTol()
{
  const _npc_fld *recordSet = reinterpret_cast<const _npc_fld *>(m_pRecordSet);
  return static_cast<unsigned int>(static_cast<int>(recordSet->m_fWindTol));
}

void CMerchant::Loop()
{
  UpdateSFCont();
  SendMsg_TransShipTicketNumInform(static_cast<unsigned int>(-1));
}

void CMerchant::OutOfSec()
{
  Destroy(nullptr);
}

void CMerchant::SendMsg_FixPosition(int n)
{

  _npc_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wLastEffectCode = m_wLastContEffect;
  FloatToShort(m_fCurPos, msg.zCur, 3);

  unsigned __int8 packetType[2] = {4, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(static_cast<unsigned int>(n), packetType, reinterpret_cast<char *>(&msg), sizeof(msg));
  SendMsg_TransShipTicketNumInform(static_cast<unsigned int>(n));
}

void CMerchant::SendMsg_RealFixPosition(bool /*bCircle*/)
{
  // this is not a stub
}

void CMerchant::SendMsg_RealMovePoint(int n)
{

  _npc_real_move_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wLastEffectCode = m_wLastContEffect;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(m_fTarPos[0]);
  msg.zTar[1] = static_cast<__int16>(m_fTarPos[2]);

  unsigned __int8 packetType[2] = {4, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(static_cast<unsigned int>(n), packetType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

bool CMerchant::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_pItemStore = nullptr;
  m_dwLastDestroyTime = 0;
  m_byRaceCode = 0;
  std::memset(m_nLeftTicketNum, -1, sizeof(m_nLeftTicketNum));
  return true;
}

int CMerchant::s_nLiveNum;
unsigned int CMerchant::s_dwSerialCnt;

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

bool CMerchant::Destroy(CGameObject * /*pAttObj*/)
{
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy();
  --s_nLiveNum;
  return CCharacter::Destroy();
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

void CMerchant::SendMsg_Destroy()
{

  _npc_destroy_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  unsigned __int8 packetType[2] = {3, 25};
  CircleReport(packetType, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CMerchant::SendMsg_Move()
{

  _npc_move_zocl msg{};
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(m_fTarPos[0]);
  msg.zTar[1] = static_cast<__int16>(m_fTarPos[2]);
  unsigned __int8 packetType[2] = {4, 6};
  CircleReport(packetType, reinterpret_cast<char *>(&msg), sizeof(msg), false);
}

void CMerchant::SendMsg_TransShipTicketNumInform(unsigned int n)
{
  if (!m_pItemStore
      || !m_pItemStore->m_pRec
      || m_pItemStore->m_pRec->m_nStore_trade != 18
      || static_cast<unsigned int>(GetObjRace()) >= 3)
  {
    return;
  }

  CTransportShip *transportShip = &g_TransportShip[GetObjRace()];

  _trans_ship_ticket_inform_zocl msg{};
  msg.dwNpcSerial = m_dwObjSerial;

  bool changed = false;
  for (int ticketIndex = 0; ticketIndex < 2; ++ticketIndex)
  {
    msg.TicketList[ticketIndex].byDirectCode = static_cast<char>(ticketIndex);
    msg.TicketList[ticketIndex].wLeftTicketNum =
      static_cast<unsigned __int16>(transportShip->m_MgrTicket[ticketIndex].nCurTicketNum);

    if (n == static_cast<unsigned int>(-1))
    {
      if (m_nLeftTicketNum[ticketIndex] != transportShip->m_MgrTicket[ticketIndex].nCurTicketNum)
      {
        changed = true;
      }
      m_nLeftTicketNum[ticketIndex] = transportShip->m_MgrTicket[ticketIndex].nCurTicketNum;
    }
  }

  if (n != static_cast<unsigned int>(-1) || changed)
  {
    unsigned __int8 packetType[2] = {33, 4};
    if (n == static_cast<unsigned int>(-1))
    {
      _sec_info *secInfo = m_pCurMap->GetSecInfo();
      const int radius = static_cast<int>(GetUseSectorRange());
      _pnt_rect rect{};
      m_pCurMap->GetRectInRadius(&rect, radius, static_cast<unsigned int>(GetCurSecNum()));
      for (int y = rect.nStarty; y <= rect.nEndy; ++y)
      {
        for (int x = rect.nStartx; x <= rect.nEndx; ++x)
        {
          const unsigned int secIndex = static_cast<unsigned int>(secInfo->m_nSecNumW * y + x);
          CObjectList *sectorList = m_pCurMap->GetSectorListPlayer(m_wMapLayerIndex, secIndex);
          if (!sectorList)
          {
            continue;
          }

          _object_list_point *node = sectorList->m_Head.m_pNext;
          while (node != &sectorList->m_Tail)
          {
            CGameObject *obj = node->m_pItem;
            node = node->m_pNext;
            if (!obj)
            {
              continue;
            }

            if (obj->GetObjRace() == GetObjRace())
            {
              g_Network.m_pProcess[0]->LoadSendMsg(
                obj->m_ObjID.m_wIndex,
                packetType,
                reinterpret_cast<char *>(&msg),
                sizeof(msg));
            }
          }
        }
      }
    }
    else
    {
      g_Network.m_pProcess[0]->LoadSendMsg(n, packetType, reinterpret_cast<char *>(&msg), sizeof(msg));
    }
  }
}
