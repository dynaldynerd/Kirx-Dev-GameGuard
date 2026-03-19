#pragma once

#include <afxwin.h>
#include "resource.h"

class LicensePopupDlg : public CDialog
{
  DECLARE_DYNAMIC(LicensePopupDlg)

public:
  explicit LicensePopupDlg(CWnd *pParent = nullptr);
  ~LicensePopupDlg() override = default;

  bool m_bOk;
  CEdit m_EditCtrl;

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int OnInitDialog() override;
  void OnOK() override;
  void OnCancel() override;

  DECLARE_MESSAGE_MAP()
};
