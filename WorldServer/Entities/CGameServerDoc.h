#pragma once

#include <afxwin.h>

#include "CInfoSheet.h"

class CGameServerDoc : public CDocument
{
public:
  CGameServerDoc();
  virtual ~CGameServerDoc() = default;

  CInfoSheet m_InfoSheet;
  CWnd *m_pwndMainFrame;
};
