#include "pch.h"

#include "CTransportShip.h"
#include "CMyTimer.h"

bool CTransportShip::InitShip(CTransportShip *pShip, CMapData *pLinkShipMap, CMapData *pLinkMainbaseMap, CMapData *pLinkPlatformMap, int nIndex)
{
  pShip->m_pLinkShipMap = pLinkShipMap;
  pShip->m_pLinkPortMap[0] = pLinkMainbaseMap;
  pShip->m_pLinkPortMap[1] = pLinkPlatformMap;
  pShip->m_byRaceCode_Layer = (unsigned char)nIndex;
  pShip->m_bAnchor = true;
  pShip->m_byDirect = 0;
  pShip->m_bHurry = false;
  pShip->m_dwNextHurryTime = 0;
  pShip->m_dwEventCreateTime = 0;
  pShip->m_tmrCheckState.BeginTimer(1000);
  return true;
}
