#pragma once
#include <afxcmn.h>

class CMainFrame : public CFrameWnd
{
protected:
    DECLARE_DYNCREATE(CMainFrame)

public:
    enum ServerStartupState
    {
        kNotStarted = 0,
        kStarting = 1,
        kStarted = 2,
    };

    CMainFrame() noexcept;
    virtual ~CMainFrame();

    virtual int PreCreateWindow(CREATESTRUCT& cs) override;
    virtual __int64 WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

    void SetServerStartupState(ServerStartupState state);
    ServerStartupState GetServerStartupState() const;
    bool IsServerStarted() const;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
    afx_msg void OnSettingsDatabase();
    afx_msg void OnSettingsAutostart();
    afx_msg void OnUpdateSettingsAutostart(CCmdUI *pCmdUI);

    DECLARE_MESSAGE_MAP()

private:
    void FinalizeShutdown();
    void RefreshWindowTitle();
    void BuildAsciiTitleToken(const char *source, char *destination, size_t destinationSize) const;
    void MakeModeName(CStringA &mode) const;
    void UpdateStatusBar();

    bool m_bExitConfirm;
    bool m_bAllowWindowClose;
    ServerStartupState m_serverStartupState;
    CStatusBar m_statusBar;
};
