#include "pch.h"

#include "CObjectSearchDlg.h"

#include <algorithm>
#include <cstring>

#include "GlobalObjects.h"
#include "resource.h"

BEGIN_MESSAGE_MAP(CObjectSearchDlg, CDialog)
  ON_CBN_SELCHANGE(1028, &CObjectSearchDlg::OnSelchangeCOMBOObjectKind)
END_MESSAGE_MAP()

CObjectSearchDlg::CObjectSearchDlg(CWnd *pParent)
  : CDialog(IDD_OBJECT_SEARCH, pParent),
    m_ID(),
    m_szCharName{},
    m_cdObjectKind(),
    m_cdObjectID(),
    m_edIndex(0),
    m_edStrCharName()
{
}

CObjectSearchDlg::~CObjectSearchDlg()
{
  // this is not a stub
}

void CObjectSearchDlg::DoDataExchange(CDataExchange *pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, 1028, m_cdObjectKind);
  DDX_Control(pDX, 1029, m_cdObjectID);
  DDX_Text(pDX, 1026, m_edIndex);
  DDX_Text(pDX, 1038, m_edStrCharName);
}

BOOL CObjectSearchDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  m_cdObjectKind.InsertString(0, _T("Character"));
  m_cdObjectKind.InsertString(1, _T("Item"));

  CGameObject *selected = CGameObject::s_pSelectObject;
  if (selected)
  {
    m_cdObjectKind.SetCurSel(selected->m_ObjID.m_byKind);
    OnSelchangeCOMBOObjectKind();
    m_cdObjectID.SetCurSel(selected->m_ObjID.m_byID);
    m_edIndex = selected->m_ObjID.m_wIndex;
  }
  else
  {
    m_cdObjectKind.SetCurSel(0);
    OnSelchangeCOMBOObjectKind();
    m_edIndex = 0;
  }

  m_szCharName[0] = '\0';
  UpdateData(FALSE);
  return TRUE;
}

void CObjectSearchDlg::OnSelchangeCOMBOObjectKind()
{
  const int kind = m_cdObjectKind.GetCurSel();
  m_cdObjectID.ResetContent();

  if (kind == 0)
  {
    m_cdObjectID.InsertString(0, _T("Player"));
    m_cdObjectID.InsertString(1, _T("Monster"));
    m_cdObjectID.InsertString(2, _T("NPC"));
    m_cdObjectID.InsertString(3, _T("Animus"));
    m_cdObjectID.InsertString(4, _T("Tower"));
    m_cdObjectID.InsertString(5, _T("Stone"));
    m_cdObjectID.InsertString(6, _T("Keeper"));
    m_cdObjectID.InsertString(7, _T("Trap"));
    m_cdObjectID.InsertString(8, _T("GravityStoneRegener"));
    m_cdObjectID.InsertString(9, _T("CircleZone"));
    m_cdObjectID.InsertString(11, _T("AutoMinePersonal"));
  }
  else if (kind == 1)
  {
    m_cdObjectID.InsertString(0, _T("ItemBox"));
    m_cdObjectID.InsertString(1, _T("DungeonGate"));
    m_cdObjectID.InsertString(2, _T("ParkingUnit"));
    m_cdObjectID.InsertString(3, _T("ReturnGate"));
    m_cdObjectID.InsertString(4, _T("GravityStone"));
  }

  m_cdObjectID.SetCurSel(0);
}

void CObjectSearchDlg::OnOK()
{
  UpdateData(TRUE);

  if (m_edStrCharName.IsEmpty())
  {
    m_ID.m_byKind = static_cast<unsigned __int8>(m_cdObjectKind.GetCurSel());
    m_ID.m_byID = static_cast<unsigned __int8>(m_cdObjectID.GetCurSel());
    m_ID.m_wIndex = static_cast<unsigned __int16>(m_edIndex);
    m_szCharName[0] = '\0';
  }
  else
  {
    CStringA charNameA(m_edStrCharName);
    const int nameLength = std::min<int>(static_cast<int>(charNameA.GetLength()), 16);
    std::memcpy(m_szCharName, charNameA.GetString(), static_cast<size_t>(nameLength));
    m_szCharName[nameLength] = '\0';
    m_ID.m_byKind = static_cast<unsigned __int8>(m_cdObjectKind.GetCurSel());
    m_ID.m_byID = static_cast<unsigned __int8>(m_cdObjectID.GetCurSel());
    m_ID.m_wIndex = static_cast<unsigned __int16>(m_edIndex);
  }

  CDialog::OnOK();
}
