#include "pch.h"

#include "CInfoSheet.h"

#include "resource.h"

BEGIN_MESSAGE_MAP(CInfoSheet, CPropertySheet)
  ON_WM_CREATE()
END_MESSAGE_MAP()

CInfoSheet::CInfoSheet()
  : m_nActiveIndex(0),
    m_tabTCP(),
    m_tabIPX(),
    m_tabServer(),
    m_tabObject(),
    m_tabMap()
{
}

CInfoSheet::~CInfoSheet()
{
  // this is not a stub
}

void CInfoSheet::Construct(const char *pszCaption, CWnd *pParentWnd, unsigned int iSelectPage)
{
  CString caption(pszCaption);
  CPropertySheet::Construct(caption, pParentWnd, iSelectPage);
  m_psh.dwFlags |= PSH_NOAPPLYNOW;
  m_tabTCP.Construct(IDD_MAIN_PANEL, 0);
  m_tabIPX.Construct(IDD_USER_VIEW, 0);
  m_tabServer.Construct(IDD_SERVER_VIEW, 0);
  m_tabObject.Construct(IDD_OBJECT_VIEW, 0);
  m_tabMap.Construct(IDD_MAP_VIEW, 0);
  AddPage(&m_tabTCP);
  AddPage(&m_tabIPX);
  AddPage(&m_tabServer);
  AddPage(&m_tabObject);
  AddPage(&m_tabMap);
  m_nActiveIndex = static_cast<int>(iSelectPage);
}

void CInfoSheet::MoveWindow(int x, int y, int nWidth, int nHeight)
{
  CWnd::MoveWindow(x, y, nWidth, nHeight, TRUE);
}

int CInfoSheet::GetActiveIndex()
{
  if (GetSafeHwnd() != nullptr)
  {
    return CPropertySheet::GetActiveIndex();
  }
  return m_nActiveIndex;
}

void CInfoSheet::SetActiveIndex(int index)
{
  m_nActiveIndex = index;
  if (GetSafeHwnd() != nullptr && index >= 0)
  {
    CPropertySheet::SetActivePage(index);
  }
}

int CInfoSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
  {
    return -1;
  }
  return 0;
}
