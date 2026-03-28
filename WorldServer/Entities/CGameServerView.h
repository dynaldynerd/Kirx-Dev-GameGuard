#pragma once

#include <afxext.h>
#include "resource.h"

class CGameServerDoc;
class CMainFrame;
class COpenDlg;

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
  afx_msg LRESULT OnStartupProgressMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnStartupCompletedMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnStartupFailedMessage(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnStartupErrorMessage(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()

private:
  CMainFrame *GetMainFrame() const;
  bool IsServerStarted() const;
  bool BeginStartupWorker();
  void ShowLoadingDialog();
  void DestroyLoadingDialog();
  void UpdateLoadingStatus(const char *statusText);
  void CompleteStartupSuccess();
  void CompleteStartupFailure();
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
  COpenDlg *m_pOpenDlg;
  bool m_bStartupInProgress;
  bool m_bStartupCompleted;
  bool m_bRuntimeViewsCreated;
};
