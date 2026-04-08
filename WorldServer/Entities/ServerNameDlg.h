#pragma once

#include <afxwin.h>

#include "resource.h"

class CServerNameDlg : public CDialog
{
  DECLARE_DYNAMIC(CServerNameDlg)

public:
  explicit CServerNameDlg(CWnd *pParent = nullptr);
  ~CServerNameDlg() override;

  CStringA GetServerNameA() const;

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int OnInitDialog() override;
  void OnOK() override;

  DECLARE_MESSAGE_MAP()

private:
  static CStringA BuildPathFromCurrentDirectory(const char *relativePath);
  static CStringA BuildWorldInfoIniPath();
  static CStringA BuildWorldInfoDirectoryPath();
  void LoadServerName();
  bool SaveServerName();

private:
  CEdit m_serverNameEdit;
  CString m_serverName;
};
