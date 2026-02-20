#include "pch.h"

#include "CInfoSheetTabs.h"

#include <algorithm>
#include <iterator>

#include "CMainThread.h"
#include "CGameObject.h"
#include "CGameServerDoc.h"
#include "CMapData.h"
#include "CMapDisplay.h"
#include "CMapOperation.h"
#include "CMonster.h"
#include "CObjectSearchDlg.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "resource.h"

namespace
{
const TCHAR *kUseText[2] = {_T("NoUse"), _T("Use")};

const TCHAR *GetObjectKindName(const CGameObject *object)
{
  if (!object)
  {
    return _T("Unknown");
  }

  if (object->m_ObjID.m_byKind == 0)
  {
    switch (object->m_ObjID.m_byID)
    {
      case 0:
        return _T("Player");
      case 1:
        return _T("Monster");
      case 2:
        return _T("NPC");
      case 3:
        return _T("Animus");
      case 4:
        return _T("Tower");
      case 5:
        return _T("Stone");
      case 6:
        return _T("Keeper");
      case 7:
        return _T("Trap");
      case 8:
        return _T("GravityStoneRegener");
      case 9:
        return _T("CircleZone");
      case 11:
        return _T("AutoMinePersonal");
      default:
        return _T("Character");
    }
  }

  if (object->m_ObjID.m_byKind == 1)
  {
    switch (object->m_ObjID.m_byID)
    {
      case 0:
        return _T("ItemBox");
      case 1:
        return _T("DungeonGate");
      case 2:
        return _T("ParkingUnit");
      case 3:
        return _T("ReturnGate");
      case 4:
        return _T("GravityStone");
      default:
        return _T("Item");
    }
  }

  return _T("Object");
}
} // namespace

BEGIN_MESSAGE_MAP(CMapTab, CPropertyPage)
  ON_BN_CLICKED(1009, &CMapTab::OnButtonMapchange)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CServerTab, CPropertyPage)
  ON_BN_CLICKED(1006, &CServerTab::OnButtonUpdateserver)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CIPXTab, CPropertyPage)
  ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CTCPTab, CPropertyPage)
  ON_WM_TIMER()
  ON_BN_CLICKED(1039, &CTCPTab::OnBUTTONMainThreadControlApply)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CObjectTab, CPropertyPage)
  ON_WM_TIMER()
  ON_BN_CLICKED(1004, &CObjectTab::OnButtonUpdate)
  ON_BN_CLICKED(1005, &CObjectTab::OnButtonDestory)
  ON_BN_CLICKED(1003, &CObjectTab::OnButtonSearch)
  ON_BN_CLICKED(1026, &CObjectTab::OnButtonAuto)
END_MESSAGE_MAP()

CMapTab::CMapTab()
  : CPropertyPage(),
    m_hMap{},
    m_trMap()
{
}

void CMapTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1005, m_trMap);
}

BOOL CMapTab::OnSetActive()
{
  g_Main.m_GameMsg.PackingMsg(0x3EFu, 0, 0, 0);
  return CPropertyPage::OnSetActive();
}

void CMapTab::UpdateTab()
{
  if (g_Main.IsExcuteService() || !m_trMap.GetSafeHwnd())
  {
    return;
  }

  m_trMap.DeleteAllItems();
  std::fill(std::begin(m_hMap), std::end(m_hMap), static_cast<HTREEITEM>(nullptr));

  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum && mapIndex < static_cast<int>(std::size(m_hMap)); ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    if (!map || !map->m_pMapSet)
    {
      continue;
    }

    CString line;
    const int useIndex = map->m_bUse ? 1 : 0;
    line.Format(_T("%dth Map : %S..(%s)"), mapIndex, map->m_pMapSet->m_strFileName, kUseText[useIndex]);
    HTREEITEM root = m_trMap.InsertItem(line, TVI_ROOT, TVI_LAST);
    m_hMap[mapIndex] = root;

    _bsp_info *bspInfo = map->GetBspInfo();
    _sec_info *secInfo = map->GetSecInfo();
    if (!bspInfo || !secInfo)
    {
      continue;
    }

    line.Format(_T("+Size : [%d][%d][%d]"), bspInfo->m_nMapMaxSize[0], bspInfo->m_nMapMaxSize[1], bspInfo->m_nMapMaxSize[2]);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("-Size : [%d][%d][%d]"), bspInfo->m_nMapMinSize[0], bspInfo->m_nMapMinSize[1], bspInfo->m_nMapMinSize[2]);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("MapSize : [%d][%d][%d]"), bspInfo->m_nMapSize[0], bspInfo->m_nMapSize[1], bspInfo->m_nMapSize[2]);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("LeafNum : %d"), bspInfo->m_nLeafNum);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("Width/Sec : %d"), 100);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("Height/Sec : %d"), 100);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("WidthSecNum : %d"), secInfo->m_nSecNumW);
    m_trMap.InsertItem(line, root, TVI_LAST);
    line.Format(_T("HeightSecNum : %d"), secInfo->m_nSecNumH);
    m_trMap.InsertItem(line, root, TVI_LAST);
  }

  HTREEITEM selectItem = m_trMap.GetRootItem();
  if (g_MapDisplay.m_bDisplayMode && g_MapDisplay.m_pActMap)
  {
    const int activeMapIndex = g_MapOper.GetMap(g_MapDisplay.m_pActMap);
    if (activeMapIndex >= 0 && activeMapIndex < static_cast<int>(std::size(m_hMap)))
    {
      selectItem = m_hMap[activeMapIndex];
    }
  }

  if (selectItem)
  {
    m_trMap.SelectItem(selectItem);
    m_trMap.EnsureVisible(selectItem);
  }
}

void CMapTab::OnButtonMapchange()
{
  if (g_Main.IsExcuteService())
  {
    return;
  }

  if (!g_MapDisplay.m_bDisplayMode)
  {
    MyMessageBox("Display Error", "Not Display Mode");
    return;
  }

  HTREEITEM selected = m_trMap.GetSelectedItem();
  if (!selected || !m_trMap.ItemHasChildren(selected))
  {
    return;
  }

  for (unsigned int mapIndex = 0; mapIndex < static_cast<unsigned int>(g_MapOper.m_nMapNum)
       && mapIndex < static_cast<unsigned int>(std::size(m_hMap));
       ++mapIndex)
  {
    if (selected == m_hMap[mapIndex])
    {
      g_Main.m_GameMsg.PackingMsg(0x3EAu, mapIndex, 0, 0);
      break;
    }
  }
}

CServerTab::CServerTab()
  : CPropertyPage(),
    m_trServer()
{
}

void CServerTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1000, m_trServer);
}

BOOL CServerTab::OnInitDialog()
{
  CPropertyPage::OnInitDialog();
  return TRUE;
}

BOOL CServerTab::OnSetActive()
{
  g_Main.m_GameMsg.PackingMsg(0x3EDu, 0, 0, 0);
  return CPropertyPage::OnSetActive();
}

void CServerTab::UpdateServerTab()
{
  if (g_Main.IsExcuteService() || !m_trServer.GetSafeHwnd())
  {
    return;
  }

  m_trServer.DeleteAllItems();

  CString line;
  HTREEITEM root = m_trServer.InsertItem(_T("World"), TVI_ROOT, TVI_LAST);

  line.Format(_T("Char : %d"), CPlayer::s_nLiveNum);
  HTREEITEM charRoot = m_trServer.InsertItem(line, root, TVI_LAST);
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    if (!map || !map->m_pMapSet)
    {
      continue;
    }
    line.Format(_T("%d : %S (%d)"), mapIndex, map->m_pMapSet->m_strCode, map->m_nMapInPlayerNum);
    m_trServer.InsertItem(line, charRoot, TVI_LAST);
  }
  m_trServer.Expand(charRoot, TVE_EXPAND);

  line.Format(_T("Monster : %d"), CMonster::s_nLiveNum);
  HTREEITEM monsterRoot = m_trServer.InsertItem(line, root, TVI_LAST);
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    if (!map || !map->m_pMapSet)
    {
      continue;
    }
    line.Format(_T("%d : %S (%d)"), mapIndex, map->m_pMapSet->m_strCode, map->m_nMapInMonsterNum);
    m_trServer.InsertItem(line, monsterRoot, TVI_LAST);
  }
  m_trServer.Expand(monsterRoot, TVE_EXPAND);
  m_trServer.Expand(root, TVE_EXPAND);
}

void CServerTab::OnButtonUpdateserver()
{
  g_Main.m_GameMsg.PackingMsg(0x3EDu, 0, 0, 0);
}

CIPXTab::CIPXTab()
  : CPropertyPage(),
    m_bUpdate(false),
    m_edAccretiaNum(0),
    m_edBellatoNum(0),
    m_edCoraNum(0)
{
}

void CIPXTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Text(pDX, 1031, m_edAccretiaNum);
  DDX_Text(pDX, 1029, m_edBellatoNum);
  DDX_Text(pDX, 1030, m_edCoraNum);
}

BOOL CIPXTab::OnSetActive()
{
  m_bUpdate = true;
  SetTimer(0, 1000, nullptr);
  return CPropertyPage::OnSetActive();
}

BOOL CIPXTab::OnKillActive()
{
  KillTimer(0);
  m_bUpdate = false;
  return CPropertyPage::OnKillActive();
}

void CIPXTab::OnTimer(UINT_PTR nIDEvent)
{
  UpdateTab();
  CPropertyPage::OnTimer(nIDEvent);
}

void CIPXTab::UpdateTab()
{
  if (g_Main.IsExcuteService() || GetSafeHwnd() == nullptr)
  {
    return;
  }

  m_edBellatoNum = CPlayer::s_nRaceNum[0];
  m_edCoraNum = CPlayer::s_nRaceNum[1];
  m_edAccretiaNum = CPlayer::s_nRaceNum[2];
  UpdateData(FALSE);
}

void CIPXTab::OnButtonUpdatelistsize()
{
}

CTCPTab::CTCPTab()
  : CPropertyPage(),
    m_bUpdate(false),
    m_btReLoadFile(),
    m_edConnSocketNum(0),
    m_edOddMsg(0),
    m_edConnectUserNum(0),
    m_edTotalUser(0),
    m_edMainThreadControlSleepIgnore(0),
    m_edMainThreadControlSleepTerm(0),
    m_edMainThreadControlSleepValue(0),
    m_edChkLoopTime(_T("X")),
    m_edChkPercent(_T("X")),
    m_edNetLoopTime(_T("X")),
    m_edNetPercent(_T("X")),
    m_edObjLoopTime(_T("X")),
    m_edObjPercent(_T("X")),
    m_edSaveFail(0)
{
}

void CTCPTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Text(pDX, 1013, m_edConnSocketNum);
  DDX_Text(pDX, 1021, m_edOddMsg);
  DDX_Text(pDX, 1018, m_edConnectUserNum);
  DDX_Text(pDX, 1022, m_edTotalUser);
  DDX_Text(pDX, 1023, m_edMainThreadControlSleepIgnore);
  DDX_Text(pDX, 1019, m_edMainThreadControlSleepTerm);
  DDX_Text(pDX, 1014, m_edMainThreadControlSleepValue);
  DDX_Text(pDX, 1038, m_edChkLoopTime);
  DDX_Text(pDX, 1040, m_edChkPercent);
  DDX_Text(pDX, 1036, m_edNetLoopTime);
  DDX_Text(pDX, 1037, m_edNetPercent);
  DDX_Text(pDX, 1027, m_edObjLoopTime);
  DDX_Text(pDX, 1035, m_edObjPercent);
  DDX_Text(pDX, 1041, m_edSaveFail);
}

BOOL CTCPTab::OnSetActive()
{
  m_bUpdate = true;
  SetTimer(0, 1000, nullptr);
  return CPropertyPage::OnSetActive();
}

BOOL CTCPTab::OnKillActive()
{
  KillTimer(0);
  m_bUpdate = false;
  return CPropertyPage::OnKillActive();
}

void CTCPTab::OnTimer(UINT_PTR nIDEvent)
{
  UpdateTab();
  CPropertyPage::OnTimer(nIDEvent);
}

void CTCPTab::UpdateTab()
{
  if (GetSafeHwnd() == nullptr)
  {
    return;
  }

  m_edConnectUserNum = CUserDB::s_nLoginNum - CPlayer::s_nLiveNum;
  m_edConnSocketNum = CPlayer::s_nLiveNum;
  m_edTotalUser = CUserDB::s_nLoginNum;

  if (!g_Main.IsExcuteService())
  {
    m_edMainThreadControlSleepIgnore = g_Main.m_nSleepIgnore;
    m_edMainThreadControlSleepTerm = g_Main.m_nSleepTerm;
    m_edMainThreadControlSleepValue = g_Main.m_nSleepValue;
    m_edSaveFail = 0;
    m_edOddMsg = g_Network.m_pProcess[0] ? g_Network.m_pProcess[0]->m_nOddMsgNum : 0;
  }

  UpdateData(FALSE);
}

void CTCPTab::OnBUTTONMainThreadControlApply()
{
  g_Main.m_GameMsg.PackingMsg(0x3F6u, 0, 0, 0);
}

void CTCPTab::OnButtonUpdatelistsize()
{
}

CObjectTab::CObjectTab()
  : CPropertyPage(),
    m_btAuto(),
    m_trObject(),
    m_bAutoUpdate(false)
{
}

void CObjectTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1026, m_btAuto);
  DDX_Control(pDX, 1025, m_trObject);
}

BOOL CObjectTab::OnSetActive()
{
  m_btAuto.SetWindowText(_T("Auto"));
  m_bAutoUpdate = false;
  g_Main.m_GameMsg.PackingMsg(0x3EEu, 0, 0, 0);
  return CPropertyPage::OnSetActive();
}

BOOL CObjectTab::OnKillActive()
{
  if (m_bAutoUpdate)
  {
    KillTimer(0);
    m_bAutoUpdate = false;
  }
  return CPropertyPage::OnKillActive();
}

void CObjectTab::OnTimer(UINT_PTR nIDEvent)
{
  g_Main.m_GameMsg.PackingMsg(0x3EEu, 0, 0, 0);
  CPropertyPage::OnTimer(nIDEvent);
}

void CObjectTab::UpdateTab()
{
  if (g_Main.IsExcuteService() || !m_trObject.GetSafeHwnd())
  {
    return;
  }

  m_trObject.DeleteAllItems();

  CGameObject *selected = CGameObject::s_pSelectObject;
  if (!selected)
  {
    m_trObject.InsertItem(_T("No selected object"), TVI_ROOT, TVI_LAST);
    return;
  }

  CString line;
  line.Format(
    _T("%s [%u:%u:%u]"),
    GetObjectKindName(selected),
    selected->m_ObjID.m_byKind,
    selected->m_ObjID.m_byID,
    selected->m_ObjID.m_wIndex);
  HTREEITEM root = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

  line.Format(_T("Live: %d  Move: %d  Corpse: %d"), selected->m_bLive ? 1 : 0, selected->m_bMove ? 1 : 0, selected->m_bCorpse ? 1 : 0);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("CurPos: %.2f, %.2f, %.2f"), selected->m_fCurPos[0], selected->m_fCurPos[1], selected->m_fCurPos[2]);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("AbsPos: %.2f, %.2f, %.2f"), selected->m_fAbsPos[0], selected->m_fAbsPos[1], selected->m_fAbsPos[2]);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("CurSector: %d"), selected->GetCurSecNum());
  m_trObject.InsertItem(line, root, TVI_LAST);

  if (selected->m_pCurMap && selected->m_pCurMap->m_pMapSet)
  {
    line.Format(_T("Map: %S"), selected->m_pCurMap->m_pMapSet->m_strCode);
    m_trObject.InsertItem(line, root, TVI_LAST);
  }

  line.Format(_T("Layer: %u"), selected->m_wMapLayerIndex);
  m_trObject.InsertItem(line, root, TVI_LAST);

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 0)
  {
    CPlayer *player = reinterpret_cast<CPlayer *>(selected);
    line.Format(_T("CharName: %S"), player->m_Param.GetCharNameA());
    m_trObject.InsertItem(line, root, TVI_LAST);
    line.Format(_T("Level: %d"), player->m_Param.GetLevel());
    m_trObject.InsertItem(line, root, TVI_LAST);
  }

  m_trObject.Expand(root, TVE_EXPAND);
}

void CObjectTab::OnButtonUpdate()
{
  g_Main.m_GameMsg.PackingMsg(0x3EEu, 0, 0, 0);
}

void CObjectTab::OnButtonDestory()
{
  if (!g_Main.IsExcuteService())
  {
    g_Main.m_GameMsg.PackingMsg(0x3F5u, 0, 0, 0);
  }
}

void CObjectTab::OnButtonSearch()
{
  if (g_Main.IsExcuteService())
  {
    return;
  }

  CObjectSearchDlg dialog;
  if (dialog.DoModal() != IDOK)
  {
    return;
  }

  CGameObject *target = nullptr;
  if (dialog.m_szCharName[0] != '\0')
  {
    target = g_Main.GetCharW(dialog.m_szCharName);
  }
  else
  {
    target = g_Main.GetObjectA(&dialog.m_ID);
  }

  if (!target || !target->m_bLive)
  {
    return;
  }

  CGameObject::s_pSelectObject = target;
  if (g_pDoc->m_InfoSheet.GetActiveIndex() == 3)
  {
    g_Main.m_GameMsg.PackingMsg(0x3EEu, 0, 0, 0);
  }

  if (g_MapDisplay.m_bDisplayMode && target->m_pCurMap)
  {
    const int mapIndex = g_MapOper.GetMap(target->m_pCurMap);
    if (mapIndex >= 0)
    {
      g_Main.m_GameMsg.PackingMsg(0x3EAu, static_cast<unsigned int>(mapIndex), 0, 0);
    }
  }
}

void CObjectTab::OnButtonAuto()
{
  if (g_Main.IsExcuteService())
  {
    return;
  }

  if (!m_bAutoUpdate)
  {
    SetTimer(0, 1000, nullptr);
    m_btAuto.SetWindowText(_T("Abort"));
  }
  else
  {
    KillTimer(0);
    m_btAuto.SetWindowText(_T("Start"));
  }

  m_bAutoUpdate = !m_bAutoUpdate;
}
