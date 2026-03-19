#include "pch.h"

#include "COpenDlg.h"

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

int COpenDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  return 1;
}

