#pragma once
#include <afxcmn.h>

class CMainFrame : public CFrameWnd
{
protected:
    DECLARE_DYNCREATE(CMainFrame)

public:
    CMainFrame() noexcept;
    virtual ~CMainFrame();

    virtual int PreCreateWindow(CREATESTRUCT& cs) override;
    virtual __int64 WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnSettingsDatabase();

    DECLARE_MESSAGE_MAP()

private:
    void FinalizeShutdown();
    void RefreshWindowTitle();
    void BuildAsciiTitleToken(const char *source, char *destination, size_t destinationSize) const;
    void MakeModeName(CStringA &mode) const;
    void UpdateStatusBar();

    bool m_bExitConfirm;
    bool m_bAllowWindowClose;
    CStatusBar m_statusBar;
};
