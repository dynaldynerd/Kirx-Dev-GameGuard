#include "pch.h"

#include "CMsgProcess.h"
#include "CMainThread.h"
#include "CGameObject.h"
#include "CMapDisplay.h"
#include "CMapData.h"
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

CMsgProcess::~CMsgProcess()
{
}

void CMsgProcess::ProcessMessage(_message *pMsg)
{
  unsigned int message = pMsg->GetMessageA();
  if (message > 10001)
  {
    return;
  }

  if (message == 10001)
  {
    g_MapDisplay.OffDisplay();
    SendMessageA(g_pDoc->m_pwndMainFrame->m_hWnd, 16, 0, 0);
    return;
  }

  switch (message - 1001)
  {
  case 0:
    g_Main.gm_DisplaymodeChange();
    return;
  case 1:
  {
    const int mapIndex = static_cast<int>(pMsg->GetKey1());
    CMapData *map = g_MapOper.GetMap(mapIndex);
    g_Main.gm_MapChange(map);
    return;
  }
  case 2:
    g_Main.gm_MonsterInit(nullptr);
    return;
  case 3:
    g_Main.gm_ObjectSelect();
    return;
  case 4:
    g_Main.gm_UpdateServer();
    return;
  case 5:
    g_Main.gm_UpdateObject();
    return;
  case 6:
    g_Main.gm_UpdateMap();
    return;
  case 8:
    g_Main.gm_DisplayAll();
    return;
  case 9:
    g_Main.gm_PreCloseAnn();
    return;
  case 11:
    g_Main.gm_ServerClose();
    return;
  case 12:
    g_Main.gm_UserExit();
    return;
  case 13:
    g_Main.gm_MainThreadControl();
    return;
  case 14:
    g_Main.gm_DungeonLoad();
    return;
  default:
    return;
  }
}
