#pragma once

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

    DECLARE_MESSAGE_MAP()

private:
    CListBox m_log;
};
