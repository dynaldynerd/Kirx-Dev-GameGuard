#pragma once

#include <afxwin.h>
#include "resource.h"

class COpenDlg : public CDialog
{
  DECLARE_DYNAMIC(COpenDlg)

public:
  explicit COpenDlg(CWnd *pParent = nullptr);
  ~COpenDlg() override;
  void SetStatusText(const char *statusText);

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int OnInitDialog() override;

  DECLARE_MESSAGE_MAP()
};
