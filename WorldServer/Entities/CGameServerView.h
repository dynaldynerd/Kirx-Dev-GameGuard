#pragma once

#include <afxext.h>
#include "resource.h"

class CGameServerDoc;
class CMainFrame;

class CGameServerView : public CFormView
{
  DECLARE_DYNCREATE(CGameServerView)

protected:
  CGameServerView();

public:
  ~CGameServerView() override;

  enum
  {
    IDD = IDD_MAIN_VIEW
  };

  CGameServerDoc *GetDocument() const;

  void OnBUTTONWorldConnect();
  void OnButtonOffplayer();
  void OnButtonDummy();
  void OnButtonCollline();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  int PreCreateWindow(CREATESTRUCT &cs) override;
  void OnInitialUpdate() override;

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnButtonStart();
  afx_msg void OnButtonDisplaymode();
  afx_msg void OnButtonDisplayall();
  afx_msg void OnButtonMonster();
  afx_msg void OnBUTTONPreClose();
  afx_msg void OnButtonLogfile();
  afx_msg void OnBUTTONServerClose();
  afx_msg void OnBUTTONHSKControl();

  DECLARE_MESSAGE_MAP()

private:
  CMainFrame *GetMainFrame() const;
  bool IsServerStarted() const;
  void EnsureRuntimeViewsCreated(CGameServerDoc *document);
  void UpdateStartupControls();

private:
  CButton m_btStart;
  CButton m_btServerClose;
  CButton m_btPreClose;
  CButton m_btMonster;
  CButton m_btDisplayAll;
  CButton m_btHSKStop;
  CButton m_btLogFile;
  bool m_bStartupInProgress;
  bool m_bStartupCompleted;
  bool m_bRuntimeViewsCreated;
};
