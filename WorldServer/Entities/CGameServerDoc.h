#pragma once

#include <afxwin.h>

#include "CDisplayView.h"
#include "CInfoSheet.h"

class CStatusBar;

class CGameServerDoc : public CDocument
{
public:
  CGameServerDoc();
  virtual ~CGameServerDoc() = default;

  void CreateDisplayView(CWnd *pWnd);
  void CreateSheetView(CWnd *pWnd);

  CDisplayView m_DisplayView;
  CInfoSheet m_InfoSheet;
  CStatusBar *m_pStatusBar;
  CWnd *m_pwndMainFrame;
  CWnd *m_pwndMainView;
};
