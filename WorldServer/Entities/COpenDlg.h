#pragma once

#include <afxwin.h>
#include "resource.h"

class COpenDlg : public CDialog
{
  DECLARE_DYNAMIC(COpenDlg)

public:
  explicit COpenDlg(CWnd *pParent = nullptr);
  ~COpenDlg() override;

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnInitDialog() override;

  DECLARE_MESSAGE_MAP()
};
