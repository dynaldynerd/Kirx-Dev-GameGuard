#pragma once

#include <afxwin.h>

#include "resource.h"

class CDatabaseSetupDlg : public CDialog
{
  DECLARE_DYNAMIC(CDatabaseSetupDlg)

public:
  explicit CDatabaseSetupDlg(CWnd *pParent = nullptr);
  ~CDatabaseSetupDlg() override;
  static bool HasRequiredSettings();
  static bool IsAutoStartEnabled();
  static bool SetAutoStartEnabled(bool enabled);

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int OnInitDialog() override;
  void OnOK() override;

  afx_msg void OnBnClickedWorldTrustedConnection();
  afx_msg void OnBnClickedBillingTrustedConnection();

  DECLARE_MESSAGE_MAP()

private:
  static CStringA BuildSettingsIniPath();
  static CStringA BuildInitializeDirectoryPath();
  static CStringA BuildWorldInfoIniPath();
  static CStringA BuildWorldInfoDirectoryPath();
  static bool IsValidIpv4Address(const CString &value);
  static bool TryParsePort(const CString &value, unsigned __int16 *port);
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
  CEdit m_accountAddressEdit;
  CEdit m_accountPortEdit;
  CEdit m_gateIpEdit;
  CEdit m_gatePortEdit;
  CString m_worldUserId;
  CString m_worldPassword;
  CString m_billingServerAddress;
  CString m_billingDbName;
  CString m_billingUserId;
  CString m_billingPassword;
  CString m_accountAddress;
  CString m_accountPort;
  CString m_gateIp;
  CString m_gatePort;
  BOOL m_worldTrustedConnection;
  BOOL m_billingTrustedConnection;
};
