#pragma once

#include <afxdlgs.h>

#include "CInfoSheetTabs.h"

class CInfoSheet : public CPropertySheet
{
public:
  CInfoSheet();
  ~CInfoSheet() override;

  void Construct(const char *pszCaption, CWnd *pParentWnd, unsigned int iSelectPage);
  void MoveWindow(int x, int y, int nWidth, int nHeight);
  int GetActiveIndex();
  void SetActiveIndex(int index);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  DECLARE_MESSAGE_MAP()

private:
  int m_nActiveIndex;

public:
  CTCPTab m_tabTCP;
  CIPXTab m_tabIPX;
  CServerTab m_tabServer;
  CObjectTab m_tabObject;
  CMapTab m_tabMap;
};
