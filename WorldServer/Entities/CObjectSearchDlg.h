#pragma once

#include <afxwin.h>

#include "CGameObject.h"

class CObjectSearchDlg : public CDialog
{
public:
  explicit CObjectSearchDlg(CWnd *pParent = nullptr);
  ~CObjectSearchDlg() override = default;

  _object_id m_ID;
  char m_szCharName[17];
  CComboBox m_cdObjectKind;
  CComboBox m_cdObjectID;
  unsigned int m_edIndex;
  CString m_edStrCharName;

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnInitDialog() override;

  afx_msg void OnSelchangeCOMBOObjectKind();
  afx_msg void OnOK();

  DECLARE_MESSAGE_MAP()
};
