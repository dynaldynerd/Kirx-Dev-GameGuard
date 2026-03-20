#include "pch.h"

#include "LicensePopupDlg.h"

IMPLEMENT_DYNAMIC(LicensePopupDlg, CDialog)

BEGIN_MESSAGE_MAP(LicensePopupDlg, CDialog)
END_MESSAGE_MAP()

LicensePopupDlg::LicensePopupDlg(CWnd *pParent)
  : CDialog(IDD_LICENSE, pParent),
    m_bOk(false),
    m_EditCtrl()
{
}

void LicensePopupDlg::DoDataExchange(CDataExchange *pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, 1032, m_EditCtrl);
}

int LicensePopupDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  HRSRC resourceInfo = FindResourceA(AfxGetInstanceHandle(), MAKEINTRESOURCEA(151), "TEXT");
  if (resourceInfo != nullptr)
  {
    HGLOBAL resourceData = LoadResource(AfxGetInstanceHandle(), resourceInfo);
    if (resourceData != nullptr)
    {
      const char *resourceText = static_cast<const char *>(LockResource(resourceData));
      if (resourceText != nullptr)
      {
        CString text(resourceText);
        m_EditCtrl.SetWindowText(text);
      }
    }
  }

  m_bOk = false;
  return TRUE;
}

void LicensePopupDlg::OnOK()
{
  m_bOk = true;
  CDialog::OnOK();
}

void LicensePopupDlg::OnCancel()
{
  m_bOk = false;
  CDialog::OnCancel();
}
