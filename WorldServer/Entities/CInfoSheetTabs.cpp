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
#include "CItemBox.h"
#include "CMonster.h"
#include "CObjectSearchDlg.h"
#include "CPlayer.h"
#include "CGravityStoneRegener.h"
#include "CCircleZone.h"
#include "CGravityStone.h"
#include "CReturnGate.h"
#include "DummyPosition.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "monster_fld.h"
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

const char *GetMapName(const CGameObject *object)
{
  if (!object || !object->m_pCurMap)
  {
    return "";
  }

  _bsp_info *bspInfo = object->m_pCurMap->GetBspInfo();
  return bspInfo ? bspInfo->m_szBspName : "";
}

const TCHAR *GetGravityStoneRegenerStateName(CGravityStoneRegener::GSR_STATE state)
{
  switch (state)
  {
    case CGravityStoneRegener::GSR_INIT:
      return _T("INIT");
    case CGravityStoneRegener::GSR_CREATE:
      return _T("CREATE");
    case CGravityStoneRegener::GSR_REGEN:
      return _T("REGEN");
    case CGravityStoneRegener::GSR_TAKE:
      return _T("TAKE");
    case CGravityStoneRegener::GSR_DESTROY:
      return _T("DESTROY");
    default:
      return _T("NONE");
  }
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
    m_edChkLoopTime(_T("")),
    m_edChkPercent(_T("")),
    m_edNetLoopTime(_T("")),
    m_edNetPercent(_T("")),
    m_edObjLoopTime(_T("")),
    m_edObjPercent(_T("")),
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
    m_trObject.InsertItem(_T("Nothing Selected Object"), TVI_ROOT, TVI_LAST);
    return;
  }

  CString line;

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 1)
  {
    CMonster *monster = static_cast<CMonster *>(selected);
    const _monster_fld *monsterRecord = static_cast<const _monster_fld *>(monster->m_pRecordSet);

    line.Format(_T("Mon-Monster - %S"), monsterRecord ? monsterRecord->m_strName : "");
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Code : %S"), selected->m_pRecordSet ? selected->m_pRecordSet->m_strCode : "");
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), selected->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Serial : %d"), selected->m_dwObjSerial);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Live : %d"), selected->m_bLive ? 1 : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("RecIndex : %d"), selected->m_pRecordSet ? selected->m_pRecordSet->m_dwIndex : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    if (monster->m_pDumPosition != nullptr)
    {
      line.Format(_T("StartDummy : %S"), monster->m_pDumPosition->m_szCode);
      m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    }

    line.Format(_T("Move : %d"), selected->m_bMove ? 1 : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("TarPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fOldPos[0]),
      static_cast<int>(selected->m_fOldPos[1]),
      static_cast<int>(selected->m_fOldPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("m_nMoveType : %d"), static_cast<int>(monster->GetMoveType()));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fCurPos[0]),
      static_cast<int>(selected->m_fCurPos[1]),
      static_cast<int>(selected->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("AbsPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fAbsPos[0]),
      static_cast<int>(selected->m_fAbsPos[1]),
      static_cast<int>(selected->m_fAbsPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("ScrPos : [%d], [%d]"), selected->m_nScreenPos[0], selected->m_nScreenPos[1]);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("CurMap : %S"), GetMapName(selected));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("CurSector : %d"), selected->GetCurSecNum());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    HTREEITEM parameterRoot = m_trObject.InsertItem(_T("Parameter"), TVI_ROOT, TVI_LAST);

    line.Format(_T("m_nHP : %d"), monster->m_nHP);
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("m_nLevel : %d"), static_cast<int>(monster->GetLevel()));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("DefSkill : %d"), static_cast<int>(selected->GetDefSkill(false)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("Head Def : %d"), static_cast<int>(selected->GetDefFC(4, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("U Body Def : %d"), static_cast<int>(selected->GetDefFC(0, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("L Body Def : %d"), static_cast<int>(selected->GetDefFC(1, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("Hand Def : %d"), static_cast<int>(selected->GetDefFC(2, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("Foot Def : %d"), static_cast<int>(selected->GetDefFC(3, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    m_trObject.Expand(parameterRoot, TVE_EXPAND);
    return;
  }

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 0)
  {
    CPlayer *player = static_cast<CPlayer *>(selected);

    line.Format(_T("Char-Player - %S"), player->m_Param.GetCharNameA());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), selected->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Serial : %d"), selected->m_dwObjSerial);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Live : %d"), selected->m_bLive ? 1 : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("RecIndex : %d"), selected->m_pRecordSet ? selected->m_pRecordSet->m_dwIndex : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Move : %d"), selected->m_bMove ? 1 : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("TarPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fOldPos[0]),
      static_cast<int>(selected->m_fOldPos[1]),
      static_cast<int>(selected->m_fOldPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("m_nMoveType : %d"), player->m_byMoveType);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fCurPos[0]),
      static_cast<int>(selected->m_fCurPos[1]),
      static_cast<int>(selected->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("CurSector : %d"), selected->GetCurSecNum());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Dalant : %u"), player->m_Param.GetDalant());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Gold : %u"), player->m_Param.GetGold());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("CurMap : %S"), GetMapName(selected));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    HTREEITEM parameterRoot = m_trObject.InsertItem(_T("Parameter"), TVI_ROOT, TVI_LAST);
    line.Format(_T("m_nSP : %u"), player->m_Param.GetSP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nHP : %u"), player->m_Param.GetHP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nFP : %u"), player->m_Param.GetFP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_dExp : %.0f"), static_cast<double>(player->m_Param.GetExp()));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nLevel : %u"), player->m_Param.GetLevel());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    m_trObject.Expand(parameterRoot, TVE_EXPAND);
    return;
  }

  if (selected->m_ObjID.m_byKind == 1 && selected->m_ObjID.m_byID == 0)
  {
    CItemBox *itemBox = static_cast<CItemBox *>(selected);
    line.Format(
      _T("Item-Box - %S"),
      GetItemKorName(itemBox->m_Item.m_byTableCode, itemBox->m_Item.m_wItemIndex));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), itemBox->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("Item Name : %S(%S)"),
      selected->m_pRecordSet ? selected->m_pRecordSet->m_strCode : "",
      GetItemKorName(itemBox->m_Item.m_byTableCode, itemBox->m_Item.m_wItemIndex));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("State : %d"), itemBox->m_nStateCode);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Dur or Amount : %u"), itemBox->m_Item.m_dwDur);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Owner : %u"), itemBox->m_dwOwnerSerial);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(itemBox->m_fCurPos[0]),
      static_cast<int>(itemBox->m_fCurPos[1]),
      static_cast<int>(itemBox->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 8)
  {
    CGravityStoneRegener *regener = static_cast<CGravityStoneRegener *>(selected);
    m_trObject.InsertItem(_T("Item-Gravity Stone Regener"), TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), regener->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Portal Index : %d"), regener->GetPortalInx());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("State : %s"), GetGravityStoneRegenerStateName(regener->m_eState));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(regener->m_fCurPos[0]),
      static_cast<int>(regener->m_fCurPos[1]),
      static_cast<int>(regener->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 9)
  {
    CCircleZone *zone = static_cast<CCircleZone *>(selected);
    m_trObject.InsertItem(_T("Item-Circle Zone"), TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), zone->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Portal Index : %d"), zone->GetPortalInx());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Color : %d"), zone->m_byColor);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(zone->m_fCurPos[0]),
      static_cast<int>(zone->m_fCurPos[1]),
      static_cast<int>(zone->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  if (selected->m_ObjID.m_byKind == 1 && selected->m_ObjID.m_byID == 4)
  {
    CGravityStone *stone = static_cast<CGravityStone *>(selected);
    m_trObject.InsertItem(_T("Item-CGravity Stone"), TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), stone->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    if (stone->m_pkOwner != nullptr)
    {
      line.Format(_T("Owner : %S"), stone->m_pkOwner->m_Param.GetCharNameA());
    }
    else
    {
      line = _T("Owner : NULL");
    }
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(stone->m_fCurPos[0]),
      static_cast<int>(stone->m_fCurPos[1]),
      static_cast<int>(stone->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  if (selected->m_ObjID.m_byKind == 1 && selected->m_ObjID.m_byID == 3)
  {
    CReturnGate *gate = static_cast<CReturnGate *>(selected);
    m_trObject.InsertItem(_T("Item-Return Gate"), TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), gate->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    if (gate->m_pkOwner != nullptr)
    {
      line.Format(_T("Owner : %S"), gate->m_pkOwner->m_Param.GetCharNameA());
    }
    else
    {
      line = _T("Owner : NULL");
    }
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(gate->m_fCurPos[0]),
      static_cast<int>(gate->m_fCurPos[1]),
      static_cast<int>(gate->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 6 && g_Keeper != nullptr)
  {
    line.Format(_T("Live : %d"), g_Keeper->m_bLive ? 1 : 0);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    line.Format(
      _T("Pos : %d, %d, %d"),
      static_cast<int>(g_Keeper->m_fCurPos[0]),
      static_cast<int>(g_Keeper->m_fCurPos[1]),
      static_cast<int>(g_Keeper->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    line.Format(_T("Master : %d"), g_Keeper->m_nMasterRace);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    return;
  }

  line.Format(
    _T("%s [%u:%u:%u]"),
    GetObjectKindName(selected),
    selected->m_ObjID.m_byKind,
    selected->m_ObjID.m_byID,
    selected->m_ObjID.m_wIndex);
  HTREEITEM root = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

  line.Format(
    _T("Live: %d  Move: %d  Corpse: %d"),
    selected->m_bLive ? 1 : 0,
    selected->m_bMove ? 1 : 0,
    selected->m_bCorpse ? 1 : 0);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("CurPos: %.2f, %.2f, %.2f"), selected->m_fCurPos[0], selected->m_fCurPos[1], selected->m_fCurPos[2]);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("AbsPos: %.2f, %.2f, %.2f"), selected->m_fAbsPos[0], selected->m_fAbsPos[1], selected->m_fAbsPos[2]);
  m_trObject.InsertItem(line, root, TVI_LAST);

  line.Format(_T("CurSector: %d"), selected->GetCurSecNum());
  m_trObject.InsertItem(line, root, TVI_LAST);
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
