#include "pch.h"

#include "COpenDlg.h"

#include "resource.h"

IMPLEMENT_DYNAMIC(COpenDlg, CDialog)

BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
END_MESSAGE_MAP()

COpenDlg::COpenDlg(CWnd *pParent)
  : CDialog(IDD_LOADING, pParent)
{
}

COpenDlg::~COpenDlg()
{
  // this is not a stub
}

void COpenDlg::DoDataExchange(CDataExchange *pDX)
{
  CDialog::DoDataExchange(pDX);
}

void COpenDlg::SetStatusText(const char *statusText)
{
  if (GetSafeHwnd() == nullptr)
  {
    return;
  }

  ::SetDlgItemTextA(m_hWnd, IDC_LOADING_STATUS, statusText != nullptr ? statusText : "");
}

int COpenDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetStatusText("Preparing startup...");
  return 1;
}

