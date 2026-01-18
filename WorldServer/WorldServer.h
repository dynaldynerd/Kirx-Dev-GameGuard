#pragma once

#include "Net/IocpCore.h"

class CWorldServerApp : public CWinApp
{
public:
    CWorldServerApp() noexcept;

    virtual BOOL InitInstance() override;
    virtual int ExitInstance() override;

    IocpCore& GetIocp() noexcept { return m_iocp; }

    DECLARE_MESSAGE_MAP()

private:
    IocpCore m_iocp;
};

extern CWorldServerApp theApp;
