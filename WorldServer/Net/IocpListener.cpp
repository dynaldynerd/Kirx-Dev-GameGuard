#include "pch.h"
#include "IocpListener.h"

IocpListener::IocpListener()
    : m_socket(INVALID_SOCKET)
{
}

IocpListener::~IocpListener()
{
    Stop();
}

bool IocpListener::Start(const std::wstring& host, unsigned short port)
{
    Stop();

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (host.empty())
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        if (InetPtonW(AF_INET, host.c_str(), &addr.sin_addr) != 1)
        {
            Stop();
            return false;
        }
    }

    if (bind(m_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
    {
        Stop();
        return false;
    }

    if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        Stop();
        return false;
    }

    return true;
}

void IocpListener::Stop()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}
