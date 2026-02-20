#pragma once
#include <afxcmn.h>

class CMainFrame : public CFrameWnd
{
protected:
    DECLARE_DYNAMIC(CMainFrame)

public:
    CMainFrame() noexcept;
    virtual ~CMainFrame();

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTabSelChange(NMHDR *pNMHDR, LRESULT *pResult);

    DECLARE_MESSAGE_MAP()

private:
    void LayoutControls(int cx, int cy);
    void ShowTabPage(int tabIndex);

    CDialog m_mainView;
    CDialog m_mainPanel;
    CDialog m_userView;
    CDialog m_serverView;
    CDialog m_objectView;
    CDialog m_mapView;
    CDialog m_logView;

    CTabCtrl m_tab;
    CStatusBar m_statusBar;
};
