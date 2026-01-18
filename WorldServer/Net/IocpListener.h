#pragma once

#include <winsock2.h>
#include <string>

class IocpListener
{
public:
    IocpListener();
    ~IocpListener();

    bool Start(const std::wstring& host, unsigned short port);
    void Stop();

    SOCKET GetSocket() const { return m_socket; }

private:
    SOCKET m_socket;
};
