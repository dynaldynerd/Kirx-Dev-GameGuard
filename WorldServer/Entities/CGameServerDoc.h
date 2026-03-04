#pragma once

#include <afxwin.h>

#include "CDisplayView.h"
#include "CInfoSheet.h"

class CStatusBar;

class CGameServerDoc : public CDocument
{
  DECLARE_DYNCREATE(CGameServerDoc)

public:
  CGameServerDoc();
  ~CGameServerDoc() override;

  BOOL OnNewDocument() override;
  void Serialize(CArchive &ar) override;

  void CreateDisplayView(CWnd *pWnd);
  void CreateSheetView(CWnd *pWnd);

  DECLARE_MESSAGE_MAP()

public:
  CDisplayView m_DisplayView;
  CInfoSheet m_InfoSheet;
  CStatusBar *m_pStatusBar;
  CWnd *m_pwndMainFrame;
  CWnd *m_pwndMainView;
};
