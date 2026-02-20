#include "pch.h"

#include "CMapView.h"

#include "CMapData.h"
#include "CMapDisplay.h"
#include "CMapOperation.h"
#include "CGameServerDoc.h"
#include "GlobalObjects.h"
#include "resource.h"

namespace
{
constexpr int kMapTreeCtrlId = 1005;
constexpr int kLoadButtonId = 1009;
}

BEGIN_MESSAGE_MAP(CMapView, CDialog)
  ON_BN_CLICKED(kLoadButtonId, &CMapView::OnBnClickedLoad)
  ON_NOTIFY(TVN_SELCHANGED, kMapTreeCtrlId, &CMapView::OnTvnSelchangedMapTree)
END_MESSAGE_MAP()

CMapView::CMapView(CWnd *pParent)
  : CDialog(IDD_MAP_VIEW, pParent)
{
}

BOOL CMapView::OnInitDialog()
{
  CDialog::OnInitDialog();
  m_treeMap.SubclassDlgItem(kMapTreeCtrlId, this);
  return TRUE;
}

void CMapView::RefreshMapTree()
{
  if (!m_treeMap.GetSafeHwnd())
  {
    return;
  }

  m_treeMap.DeleteAllItems();
  if (!g_MapOper.m_Map || g_MapOper.m_nMapNum <= 0)
  {
    return;
  }

  for (int mapIndex = 0; mapIndex < g_MapOper.m_nMapNum; ++mapIndex)
  {
    CMapData *map = g_MapOper.GetMap(mapIndex);
    if (!map || !map->m_pMapSet)
    {
      continue;
    }

    CString mapText;
    mapText.Format(L"%dth Map : %S", mapIndex, map->m_pMapSet->m_strCode);
    HTREEITEM item = m_treeMap.InsertItem(mapText, TVI_ROOT, TVI_LAST);
    m_treeMap.SetItemData(item, static_cast<DWORD_PTR>(mapIndex + 1));
  }

  HTREEITEM firstItem = m_treeMap.GetRootItem();
  if (firstItem)
  {
    m_treeMap.SelectItem(firstItem);
    m_treeMap.EnsureVisible(firstItem);
  }
}

bool CMapView::LoadSelectedMap()
{
  return ActivateMap(GetSelectedMap());
}

void CMapView::OnBnClickedLoad()
{
  LoadSelectedMap();
}

void CMapView::OnTvnSelchangedMapTree(NMHDR * /*pNMHDR*/, LRESULT *pResult)
{
  if (g_MapDisplay.m_bDisplayMode)
  {
    CMapData *map = GetSelectedMap();
    if (map)
    {
      g_MapDisplay.ChangeMap(map);
    }
  }

  *pResult = 0;
}

CMapData *CMapView::GetSelectedMap() const
{
  if (!m_treeMap.GetSafeHwnd())
  {
    return nullptr;
  }

  HTREEITEM selected = m_treeMap.GetSelectedItem();
  if (!selected)
  {
    return nullptr;
  }

  DWORD_PTR data = m_treeMap.GetItemData(selected);
  if (data == 0)
  {
    return nullptr;
  }

  const int mapIndex = static_cast<int>(data - 1);
  if (mapIndex < 0 || mapIndex >= g_MapOper.m_nMapNum)
  {
    return nullptr;
  }

  return g_MapOper.GetMap(mapIndex);
}

bool CMapView::ActivateMap(CMapData *map)
{
  if (!map)
  {
    return false;
  }

  if (!g_MapDisplay.m_bDisplayMode)
  {
    if (!g_MapDisplay.OnDisplay(map, 0))
    {
      return false;
    }

    CRect drawRect;
    g_pDoc->m_DisplayView.GetDrawableRect(&drawRect);
    g_MapDisplay.InitCollLine(&drawRect);
    g_MapDisplay.InitDummy(&drawRect);
    return true;
  }

  return g_MapDisplay.ChangeMap(map) != 0;
}
