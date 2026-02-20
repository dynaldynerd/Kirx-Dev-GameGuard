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
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnButtonDisplayAll();
    afx_msg void OnButtonMonster();
    afx_msg void OnButtonAction();
    afx_msg void OnButtonServerClose();
    afx_msg void OnButtonDungeonLoad();
    afx_msg LRESULT OnUiUpdateMap(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUiUpdateServer(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUiUpdateObject(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    void LayoutControls(int cx, int cy);
    void MakeModeName(CStringA &mode) const;
    void UpdateStatusBar();

    CDialog m_mainView;
    CStatusBar m_statusBar;
};
