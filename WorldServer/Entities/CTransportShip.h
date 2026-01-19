#pragma once

#include "IdaCompat.h"
#include "CMapData.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CPlayer;

struct _TicketItem_fld;

class __cppobj CTransportShip
{
public:
  struct __cppobj __declspec(align(8)) __mgr_member
  {
    CPlayer *pPtr;
    unsigned int dwSerial;
  };

  struct __cppobj __mgr_ticket
  {
    int nCurTicketNum;
    int nReserTicketNum;
    unsigned int dwNextUpdateTime;
    _TicketItem_fld *pLinkTicketItem;
  };

  bool m_bAnchor;
  unsigned __int8 m_byDirect;
  bool m_bHurry;
  unsigned int m_dwNextHurryTime;
  unsigned int m_dwEventCreateTime;
  unsigned __int8 m_byRaceCode_Layer;
  CMapData *m_pLinkShipMap;
  CMapData *m_pLinkPortMap[2];
  CMyTimer m_tmrCheckState;
  __mgr_member m_NewMember[2532];
  __mgr_member m_OldMember[2532];
  CNetIndexList m_listLogoffMember;
  __mgr_ticket m_MgrTicket[2];
};
