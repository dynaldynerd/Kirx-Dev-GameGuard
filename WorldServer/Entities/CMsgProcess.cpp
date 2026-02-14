#include "pch.h"

#include "CMsgProcess.h"
#include "CMainThread.h"
#include "CMapDisplay.h"
#include "CMapOperation.h"
#include "CGameServerDoc.h"
#include "GlobalObjects.h"

CMsgProcess::CMsgProcess()
  : CMsgData()
{
}

CMsgProcess::CMsgProcess(int nObjNum)
  : CMsgData(nObjNum)
{
}

void CMsgProcess::ProcessMessage(_message *pMsg)
{
  unsigned int message = pMsg->GetMessageA();
  if (message > 0x2711)
    return;

  if (message == 10001)
  {
    g_MapDisplay.OffDisplay();
    SendMessageA(g_pDoc->m_pwndMainFrame->m_hWnd, 0x10u, 0, 0);
    return;
  }

  switch (message - 1001)
  {
  case 2:
    g_Main.gm_MonsterInit(nullptr);
    return;
  case 11:
    g_Main.gm_ServerClose();
    return;
  case 13:
    return;
  default:
    return;
  }
}
