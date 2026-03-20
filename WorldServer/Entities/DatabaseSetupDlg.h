#pragma once

#include <afxwin.h>

#include "resource.h"

class CDatabaseSetupDlg : public CDialog
{
  DECLARE_DYNAMIC(CDatabaseSetupDlg)

public:
  explicit CDatabaseSetupDlg(CWnd *pParent = nullptr);
  ~CDatabaseSetupDlg() override;

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int OnInitDialog() override;
  void OnOK() override;

  afx_msg void OnBnClickedWorldTrustedConnection();
  afx_msg void OnBnClickedBillingTrustedConnection();

  DECLARE_MESSAGE_MAP()

private:
  CStringA BuildIniPath() const;
  CStringA BuildInitializeDirectoryPath() const;
  void LoadSettings();
  bool SaveSettings();
  void UpdateCredentialControls();

private:
  CButton m_worldTrustedConnectionCheck;
  CEdit m_worldUserEdit;
  CEdit m_worldPasswordEdit;
  CEdit m_billingServerEdit;
  CEdit m_billingDbEdit;
  CButton m_billingTrustedConnectionCheck;
  CEdit m_billingUserEdit;
  CEdit m_billingPasswordEdit;
  CString m_worldUserId;
  CString m_worldPassword;
  CString m_billingServerAddress;
  CString m_billingDbName;
  CString m_billingUserId;
  CString m_billingPassword;
  BOOL m_worldTrustedConnection;
  BOOL m_billingTrustedConnection;
};
