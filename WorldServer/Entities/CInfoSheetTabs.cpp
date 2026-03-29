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
#include "WeaponItem_fld.h"
#include "WorldServerUtil.h"
#include "monster_fld.h"
#include "resource.h"
#include "skill_fld.h"

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

CMapTab::~CMapTab()
{
  // this is not a stub
}

void CMapTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1005, m_trMap);
}

int CMapTab::OnSetActive()
{
  g_Main.m_GameMsg.PackingMsg(1007, 0, 0, 0);
  const int result = static_cast<int>(CPropertyPage::OnSetActive());
  return result;
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
      g_Main.m_GameMsg.PackingMsg(1002, mapIndex, 0, 0);
      break;
    }
  }
}

CServerTab::CServerTab()
  : CPropertyPage(),
    m_trServer()
{
}

CServerTab::~CServerTab()
{
  // this is not a stub
}

void CServerTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1000, m_trServer);
}

int CServerTab::OnInitDialog()
{
  CPropertyPage::OnInitDialog();
  return TRUE;
}

int CServerTab::OnSetActive()
{
  g_Main.m_GameMsg.PackingMsg(1005, 0, 0, 0);
  const int result = static_cast<int>(CPropertyPage::OnSetActive());
  return result;
}

void CServerTab::UpdateServerTab()
{
  if (g_Main.IsExcuteService())
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
    line.Format(_T("%d : %S (%d)"), mapIndex, map->m_pMapSet->m_strCode, map->m_nMapInPlayerNum);
    m_trServer.InsertItem(line, charRoot, TVI_LAST);
  }
  m_trServer.Expand(charRoot, TVE_EXPAND);

  line.Format(_T("Monster : %d"), CMonster::s_nLiveNum);
  HTREEITEM monsterRoot = m_trServer.InsertItem(line, root, TVI_LAST);
  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    line.Format(_T("%d : %S (%d)"), mapIndex, map->m_pMapSet->m_strCode, map->m_nMapInMonsterNum);
    m_trServer.InsertItem(line, monsterRoot, TVI_LAST);
  }
  m_trServer.Expand(monsterRoot, TVE_EXPAND);
  m_trServer.Expand(root, TVE_EXPAND);
}

void CServerTab::OnButtonUpdateserver()
{
  g_Main.m_GameMsg.PackingMsg(1005, 0, 0, 0);
}

CIPXTab::CIPXTab()
  : CPropertyPage(),
    m_bUpdate(false),
    m_edAccretiaNum(0),
    m_edBellatoNum(0),
    m_edCoraNum(0)
{
}

CIPXTab::~CIPXTab()
{
  // this is not a stub
}

void CIPXTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Text(pDX, 1031, m_edAccretiaNum);
  DDX_Text(pDX, 1029, m_edBellatoNum);
  DDX_Text(pDX, 1030, m_edCoraNum);
}

int CIPXTab::OnSetActive()
{
  m_bUpdate = true;
  SetTimer(0, 1000, nullptr);
  const int result = static_cast<int>(CPropertyPage::OnSetActive());
  return result;
}

int CIPXTab::OnKillActive()
{
  KillTimer(0);
  m_bUpdate = false;
  const int result = static_cast<int>(CPropertyPage::OnKillActive());
  return result;
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

CTCPTab::~CTCPTab()
{
  // this is not a stub
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

int CTCPTab::OnSetActive()
{
  m_bUpdate = true;
  SetTimer(0, 1000, nullptr);
  const int result = static_cast<int>(CPropertyPage::OnSetActive());
  return result;
}

int CTCPTab::OnKillActive()
{
  KillTimer(0);
  m_bUpdate = false;
  const int result = static_cast<int>(CPropertyPage::OnKillActive());
  return result;
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
  g_Main.m_GameMsg.PackingMsg(1014, 0, 0, 0);
}

void CTCPTab::OnButtonUpdatelistsize()
{
}

CObjectTab::CObjectTab()
  : CPropertyPage(),
    m_btAuto(),
    m_trObject(),
    m_bAutoUpdate(false),
    m_pLastDisplayedObject(nullptr)
{
}

CObjectTab::~CObjectTab()
{
  // this is not a stub
}

void CObjectTab::DoDataExchange(CDataExchange *pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, 1026, m_btAuto);
  DDX_Control(pDX, 1025, m_trObject);
}

int CObjectTab::OnSetActive()
{
  m_btAuto.SetWindowText(_T("Auto"));
  m_bAutoUpdate = false;
  g_Main.m_GameMsg.PackingMsg(1006, 0, 0, 0);
  const int result = static_cast<int>(CPropertyPage::OnSetActive());
  return result;
}

int CObjectTab::OnKillActive()
{
  if (m_bAutoUpdate)
  {
    KillTimer(0);
    m_bAutoUpdate = false;
  }
  const int result = static_cast<int>(CPropertyPage::OnKillActive());
  return result;
}

void CObjectTab::OnTimer(UINT_PTR nIDEvent)
{
  g_Main.m_GameMsg.PackingMsg(1006, 0, 0, 0);
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
  const bool objectChanged = selected != m_pLastDisplayedObject;
  auto finishUpdate = [this, selected, objectChanged]()
  {
    m_pLastDisplayedObject = selected;
    if (!objectChanged)
    {
      return;
    }

    HTREEITEM firstItem = m_trObject.GetRootItem();
    if (firstItem != nullptr)
    {
      m_trObject.EnsureVisible(firstItem);
    }
  };
  if (!selected)
  {
    m_trObject.InsertItem(_T("Nothing Selected Object"), TVI_ROOT, TVI_LAST);
    finishUpdate();
    return;
  }

  CString line;
  static const char *kEquipUnitName[3] = {"Normal", "Weapon", "Unit"};
  static const char *kFixPartName[8] = {"Upper", "Lower", "Glove", "Shoes", "Shield", "Weapon", "Helmet", "Accessory"};

  if (selected->m_ObjID.m_byKind == 0 && selected->m_ObjID.m_byID == 0)
  {
    CPlayer *player = static_cast<CPlayer *>(selected);

    line.Format(_T("Char-Player - %S"), player->m_Param.GetCharNameA());
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Degree : %d"), static_cast<int>(player->m_byUserDgr));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("Index : %d"), selected->m_ObjID.m_wIndex);
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

    line.Format(_T("m_nMoveType : %d"), static_cast<int>(player->m_byMoveType));
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

    const int attackToolType = static_cast<int>(player->m_pmWpn.GetAttackToolType());
    const char *attackToolName = attackToolType >= 0 && attackToolType < static_cast<int>(std::size(kEquipUnitName))
      ? kEquipUnitName[attackToolType]
      : "Unknown";
    line.Format(_T("Attack Tool : %S"), attackToolName);
    HTREEITEM attackRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("nGaMinAF(%d), nGaMaxAF(%d), nMaMaxAF(%d), nMaMinAF(%d)"),
      player->m_pmWpn.nGaMinAF,
      player->m_pmWpn.nGaMaxAF,
      player->m_pmWpn.nMaMaxAF,
      player->m_pmWpn.nMaMinAF);
    m_trObject.InsertItem(line, attackRoot, TVI_LAST);

    line.Format(
      _T("byGaMinSel(%d), byGaMaxSel(%d), byMaMinSel(%d), byMaMaxSel(%d)"),
      player->m_pmWpn.byGaMinSel,
      player->m_pmWpn.byGaMaxSel,
      player->m_pmWpn.byMaMinSel,
      player->m_pmWpn.byMaMaxSel);
    m_trObject.InsertItem(line, attackRoot, TVI_LAST);

    line.Format(
      _T("byAttTolType(%d), byWpClass(%d), byWpType(%d)"),
      player->m_pmWpn.byAttTolType,
      player->m_pmWpn.byWpClass,
      player->m_pmWpn.byWpType);
    m_trObject.InsertItem(line, attackRoot, TVI_LAST);

    line.Format(_T("wGaAttRange(%d), wMaAttRange(%d)"), player->m_pmWpn.wGaAttRange, player->m_pmWpn.wMaAttRange);
    m_trObject.InsertItem(line, attackRoot, TVI_LAST);

    if (player->m_pmWpn.pFixWp)
    {
      _WeaponItem_fld *weaponRecord =
        reinterpret_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(player->m_pmWpn.pFixWp->m_wItemIndex));
      line.Format(_T("FixWeapon : %S"), weaponRecord ? weaponRecord->m_strName : "NULL");
    }
    else if (player->m_pmWpn.pFixUnit)
    {
      _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(player->m_pmWpn.pFixUnit->byFrame);
      line.Format(_T("FixUnit : %S"), record ? record->m_strCode : "NULL");
    }
    else
    {
      line = _T("FixWeapon : NULL");
    }
    m_trObject.InsertItem(line, attackRoot, TVI_LAST);

    HTREEITEM equipRoot = m_trObject.InsertItem(_T("Equip"), TVI_ROOT, TVI_LAST);
    for (unsigned int slot = 0; slot < std::size(player->m_Param.m_dbEquip.m_List); ++slot)
    {
      _STORAGE_LIST::_db_con *equipItem = &player->m_Param.m_dbEquip.m_List[slot];
      const unsigned __int8 effectCode = player->GetEffectEquipCode(1u, static_cast<unsigned __int8>(slot));
      const char *partName = slot < std::size(kFixPartName) ? kFixPartName[slot] : "Unknown";
      if (equipItem->m_bLoad)
      {
        _base_fld *record = g_Main.m_tblItemData[equipItem->m_byTableCode].GetRecord(equipItem->m_wItemIndex);
        const char *upgInfo = DisplayItemUpgInfo(equipItem->m_byTableCode, static_cast<int>(equipItem->m_dwLv));
        line.Format(
          _T("%d: [%S]%S, [sr]%d, [dr]%I64u, [lv]%S"),
          effectCode,
          partName,
          record ? record->m_strCode : "NULL",
          equipItem->m_wSerial,
          equipItem->m_dwDur,
          upgInfo ? upgInfo : "");
      }
      else
      {
        line.Format(_T("%d: [%S]:def"), effectCode, partName);
      }
      m_trObject.InsertItem(line, equipRoot, TVI_LAST);
    }

    const int wearUsed = static_cast<int>(std::size(player->m_Param.m_dbEmbellish.m_List))
      - player->m_Param.m_dbEmbellish.GetNumEmptyCon();
    line.Format(_T("Wear(%d)"), wearUsed);
    HTREEITEM wearRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    for (unsigned int slot = 0; slot < std::size(player->m_Param.m_dbEmbellish.m_List); ++slot)
    {
      _STORAGE_LIST::_db_con *wearItem = &player->m_Param.m_dbEmbellish.m_List[slot];
      if (!wearItem->m_bLoad)
      {
        continue;
      }
      _base_fld *record = g_Main.m_tblItemData[wearItem->m_byTableCode].GetRecord(wearItem->m_wItemIndex);
      const unsigned __int8 effectCode = player->GetEffectEquipCode(2u, static_cast<unsigned __int8>(slot));
      line.Format(
        _T("%d: %S, [sr]%d"),
        effectCode,
        record ? record->m_strCode : "NULL",
        wearItem->m_wSerial);
      m_trObject.InsertItem(line, wearRoot, TVI_LAST);
    }

    const int bagSlotCount = (std::min)(
      static_cast<int>(std::size(player->m_Param.m_dbInven.m_List)),
      static_cast<int>(player->m_Param.GetBagNum()) * 20);
    const int invenUsed = bagSlotCount - player->m_Param.m_dbInven.GetNumEmptyCon();
    line.Format(_T("Inven(%d)"), invenUsed);
    HTREEITEM invenRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    for (int slot = 0; slot < bagSlotCount; ++slot)
    {
      _STORAGE_LIST::_db_con *invenItem = &player->m_Param.m_dbInven.m_List[slot];
      if (!invenItem->m_bLoad)
      {
        continue;
      }
      _base_fld *record = g_Main.m_tblItemData[invenItem->m_byTableCode].GetRecord(invenItem->m_wItemIndex);
      const char *upgInfo = DisplayItemUpgInfo(invenItem->m_byTableCode, static_cast<int>(invenItem->m_dwLv));
      line.Format(
        _T("%S, [amt]%I64u, [sr]%d, [lv]%S"),
        record ? record->m_strCode : "NULL",
        invenItem->m_dwDur,
        invenItem->m_wSerial,
        upgInfo ? upgInfo : "");
      m_trObject.InsertItem(line, invenRoot, TVI_LAST);
    }

    const int forceUsed = static_cast<int>(std::size(player->m_Param.m_dbForce.m_List))
      - player->m_Param.m_dbForce.GetNumEmptyCon();
    line.Format(_T("Force(%d)"), forceUsed);
    HTREEITEM forceRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    for (unsigned int slot = 0; slot < std::size(player->m_Param.m_dbForce.m_List); ++slot)
    {
      _STORAGE_LIST::_db_con *forceItem = &player->m_Param.m_dbForce.m_List[slot];
      if (!forceItem->m_bLoad)
      {
        continue;
      }
      _base_fld *record = g_Main.m_tblItemData[forceItem->m_byTableCode].GetRecord(forceItem->m_wItemIndex);
      line.Format(
        _T("%S, [amt]%I64u, [sr]%d"),
        record ? record->m_strCode : "NULL",
        forceItem->m_dwDur,
        forceItem->m_wSerial);
      m_trObject.InsertItem(line, forceRoot, TVI_LAST);
    }

    if (player->m_Param.GetRaceCode() == 1)
    {
      const int animusUsed = static_cast<int>(std::size(player->m_Param.m_dbAnimus.m_List))
        - player->m_Param.m_dbAnimus.GetNumEmptyCon();
      line.Format(_T("Animus(%d)"), animusUsed);
      HTREEITEM animusRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
      for (unsigned int slot = 0; slot < std::size(player->m_Param.m_dbAnimus.m_List); ++slot)
      {
        _STORAGE_LIST::_db_con *animusItem = &player->m_Param.m_dbAnimus.m_List[slot];
        if (!animusItem->m_bLoad)
        {
          continue;
        }
        _base_fld *record = g_Main.m_tblItemData[animusItem->m_byTableCode].GetRecord(animusItem->m_wItemIndex);
        const unsigned __int16 hp = static_cast<unsigned __int16>(animusItem->m_dwLv & 0xFFFFu);
        const unsigned __int16 fp = static_cast<unsigned __int16>((animusItem->m_dwLv >> 16) & 0xFFFFu);
        line.Format(
          _T("%S, [exp]%I64u, [hp]%d, [fp]%d, [sr]%d"),
          record ? record->m_strCode : "NULL",
          animusItem->m_dwDur,
          hp,
          fp,
          animusItem->m_wSerial);
        m_trObject.InsertItem(line, animusRoot, TVI_LAST);
      }
    }

    const int resBufferCount = static_cast<unsigned __int8>(player->m_Param.GetResBufferNum());
    line.Format(_T("ResBuffer(%d)"), resBufferCount);
    HTREEITEM resRoot = m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    if (player->m_Param.m_wCuttingResBuffer)
    {
      for (int resourceIndex = 0; resourceIndex < GetMaxResKind(); ++resourceIndex)
      {
        const unsigned __int16 amount = player->m_Param.m_wCuttingResBuffer[resourceIndex];
        if (!amount)
        {
          continue;
        }
        _base_fld *record = g_Main.m_tblItemData[18].GetRecord(resourceIndex);
        line.Format(_T("[code]%S, [num]%d"), record ? record->m_strCode : "NULL", amount);
        m_trObject.InsertItem(line, resRoot, TVI_LAST);
      }
    }

    HTREEITEM masteryRoot = m_trObject.InsertItem(_T("Mastery"), TVI_ROOT, TVI_LAST);
    for (unsigned int masteryIndex = 0; masteryIndex < std::size(player->m_pmMst.m_mtyWp); ++masteryIndex)
    {
      const int cum = static_cast<int>(player->m_pmMst.GetCumPerMast(0, static_cast<unsigned __int8>(masteryIndex)));
      const int mastery = static_cast<int>(player->m_pmMst.GetMasteryPerMast(0, static_cast<unsigned __int8>(masteryIndex)));
      line.Format(_T("Weapon Mp.. %d) cum: %d, mp: %d"), masteryIndex, cum, mastery);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    {
      const int cum = static_cast<int>(player->m_pmMst.GetCumPerMast(1u, 0));
      const int mastery = static_cast<int>(player->m_pmMst.GetMasteryPerMast(1u, 0));
      line.Format(_T("Suffer Mp.. 0) cum: %d, mp: %d"), cum, mastery);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    {
      const int cum = static_cast<int>(player->m_pmMst.GetCumPerMast(2u, 0));
      const int mastery = static_cast<int>(player->m_pmMst.GetMasteryPerMast(2u, 0));
      line.Format(_T("Shield Mp.. 0) cum: %d, mp: %d"), cum, mastery);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    for (unsigned int masteryIndex = 0; masteryIndex < std::size(player->m_pmMst.m_mtySkill); ++masteryIndex)
    {
      const int cum = static_cast<int>(player->m_pmMst.GetCumPerMast(3u, static_cast<unsigned __int8>(masteryIndex)));
      const int mastery = static_cast<int>(player->m_pmMst.GetMasteryPerMast(3u, static_cast<unsigned __int8>(masteryIndex)));
      line.Format(_T("Skill Mp.. %d) cum: %d, mp: %d"), masteryIndex, cum, mastery);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    for (unsigned int masteryIndex = 0; masteryIndex < std::size(player->m_pmMst.m_mtyForce); ++masteryIndex)
    {
      const int cum = static_cast<int>(player->m_pmMst.GetCumPerMast(4u, static_cast<unsigned __int8>(masteryIndex)));
      const int mastery = static_cast<int>(player->m_pmMst.GetMasteryPerMast(4u, static_cast<unsigned __int8>(masteryIndex)));
      line.Format(_T("Force Mp.. %d) cum: %d, mp: %d"), masteryIndex, cum, mastery);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    line.Format(_T("Staff Mp.. mp: %d"), static_cast<int>(player->m_pmMst.m_mtyStaff));
    m_trObject.InsertItem(line, masteryRoot, TVI_LAST);

    for (unsigned int skillIndex = 0; skillIndex < std::size(player->m_pmMst.m_lvSkill); ++skillIndex)
    {
      const int cum = static_cast<int>(player->m_pmMst.m_BaseCum.m_dwSkillCum[skillIndex]);
      const int skillLv = static_cast<int>(player->m_pmMst.GetSkillLv(static_cast<unsigned __int8>(skillIndex)));
      line.Format(_T("Skill Lv.. %d) cum: %d, lv: %d"), skillIndex, cum, skillLv);
      m_trObject.InsertItem(line, masteryRoot, TVI_LAST);
    }

    HTREEITEM sfRoot = m_trObject.InsertItem(_T("SFCont"), TVI_ROOT, TVI_LAST);
    const unsigned int sfContCurTime = _sf_continous::GetSFContCurTime();
    for (unsigned int contCode = 0; contCode < std::size(player->m_SFCont); ++contCode)
    {
      for (unsigned int slot = 0; slot < std::size(player->m_SFCont[contCode]); ++slot)
      {
        const _sf_continous *cont = &player->m_SFCont[contCode][slot];
        if (!cont->m_bExist)
        {
          continue;
        }
        _skill_fld *effectRecord = nullptr;
        if (cont->m_byEffectCode < 4)
        {
          effectRecord =
            reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[cont->m_byEffectCode].GetRecord(cont->m_wEffectIndex));
        }
        const unsigned int passed = sfContCurTime - cont->m_dwStartSec;
        if (effectRecord)
        {
          line.Format(_T("%S : pass:%d, dur:%d"), effectRecord->m_strKorName, passed, cont->m_wDurSec);
        }
        else
        {
          line.Format(
            _T("UNKNOWN(%d:%d) : pass:%d, dur:%d"),
            static_cast<int>(cont->m_byEffectCode),
            static_cast<int>(cont->m_wEffectIndex),
            passed,
            cont->m_wDurSec);
        }
        m_trObject.InsertItem(line, sfRoot, TVI_LAST);
      }
    }

    line.Format(
      _T("Tol : Water(%d), Fire(%d), Soil(%d), Wind(%d)"),
      static_cast<int>(selected->GetWaterTol()),
      static_cast<int>(selected->GetFireTol()),
      static_cast<int>(selected->GetSoilTol()),
      static_cast<int>(selected->GetWindTol()));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(_T("O : %d, Y : %d"), player->m_pmWpn.nGaMaxAF, player->m_pmWpn.nMaMaxAF);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    HTREEITEM parameterRoot = m_trObject.InsertItem(_T("Parameter"), TVI_ROOT, TVI_LAST);
    line.Format(_T("m_nST : %u"), player->m_Param.GetSP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nHP : %u"), player->m_Param.GetHP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nFP : %u"), player->m_Param.GetFP());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_dExp : %f"), static_cast<double>(player->m_Param.GetExp()));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);
    line.Format(_T("m_nLevel : %u"), player->m_Param.GetLevel());
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    m_trObject.Expand(attackRoot, TVE_EXPAND);
    m_trObject.Expand(equipRoot, TVE_EXPAND);
    m_trObject.Expand(wearRoot, TVE_EXPAND);
    m_trObject.Expand(invenRoot, TVE_EXPAND);
    m_trObject.Expand(forceRoot, TVE_EXPAND);
    m_trObject.Expand(resRoot, TVE_EXPAND);
    m_trObject.Expand(masteryRoot, TVE_EXPAND);
    m_trObject.Expand(sfRoot, TVE_EXPAND);
    m_trObject.Expand(parameterRoot, TVE_EXPAND);
    finishUpdate();
    return;
  }

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

    line.Format(_T("D Body Def : %d"), static_cast<int>(selected->GetDefFC(1, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("Hand Def : %d"), static_cast<int>(selected->GetDefFC(2, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    line.Format(_T("Foot Def : %d"), static_cast<int>(selected->GetDefFC(3, nullptr, nullptr)));
    m_trObject.InsertItem(line, parameterRoot, TVI_LAST);

    m_trObject.Expand(parameterRoot, TVE_EXPAND);
    finishUpdate();
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
    finishUpdate();
    return;
  }

  if (selected->m_ObjID.m_byKind == 1 && selected->m_ObjID.m_byID == 1)
  {
    line.Format(_T("Index : %d"), selected->m_ObjID.m_wIndex);
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("Item Name : %S(%S)"),
      selected->m_pRecordSet ? selected->m_pRecordSet->m_strCode : "NULL",
      selected->m_pRecordSet ? selected->m_pRecordSet[2].m_strCode : "NULL");
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    const unsigned __int8 hurry = *reinterpret_cast<unsigned __int8 *>(&(selected[1].m_fAbsPos[2]));
    line.Format(_T("hurry : %d"), static_cast<int>(hurry));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);

    line.Format(
      _T("CurPos : [%d], [%d], [%d]"),
      static_cast<int>(selected->m_fCurPos[0]),
      static_cast<int>(selected->m_fCurPos[1]),
      static_cast<int>(selected->m_fCurPos[2]));
    m_trObject.InsertItem(line, TVI_ROOT, TVI_LAST);
    finishUpdate();
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
    finishUpdate();
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
    finishUpdate();
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
    finishUpdate();
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
  finishUpdate();
}

void CObjectTab::OnButtonUpdate()
{
  g_Main.m_GameMsg.PackingMsg(1006, 0, 0, 0);
}

void CObjectTab::OnButtonDestory()
{
  if (!g_Main.IsExcuteService())
  {
    g_Main.m_GameMsg.PackingMsg(1013, 0, 0, 0);
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

  CGameObject *target = g_Main.GetObjectExpand(&dialog.m_ID, dialog.m_szCharName, dialog.m_edIndex);

  if (!target || !target->m_bLive)
  {
    return;
  }

  CGameObject::s_pSelectObject = target;
  if (g_pDoc->m_InfoSheet.GetActiveIndex() == 3)
  {
    g_Main.m_GameMsg.PackingMsg(1006, 0, 0, 0);
  }

  if (g_MapDisplay.m_bDisplayMode && target->m_pCurMap)
  {
    const int mapIndex = g_MapOper.GetMap(target->m_pCurMap);
    if (mapIndex >= 0)
    {
      g_Main.m_GameMsg.PackingMsg(1002, static_cast<unsigned int>(mapIndex), 0, 0);
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
