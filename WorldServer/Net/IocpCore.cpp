#include "pch.h"
#include "IocpCore.h"

IocpCore::IocpCore()
    : m_iocp(nullptr), m_running(false)
{
}

IocpCore::~IocpCore()
{
    Stop();
}

bool IocpCore::Start(unsigned int workerCount)
{
    if (m_running.load())
    {
        return true;
    }

    m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (m_iocp == nullptr)
    {
        return false;
    }

    if (workerCount == 0)
    {
        SYSTEM_INFO info{};
        GetSystemInfo(&info);
        workerCount = info.dwNumberOfProcessors;
        if (workerCount == 0)
        {
            workerCount = 1;
        }
    }

    m_running.store(true);
    m_threads.reserve(workerCount);
    for (unsigned int i = 0; i < workerCount; ++i)
    {
        HANDLE thread = CreateThread(nullptr, 0, &IocpCore::WorkerThunk, this, 0, nullptr);
        if (thread != nullptr)
        {
            m_threads.push_back(thread);
        }
    }

    return !m_threads.empty();
}

void IocpCore::Stop()
{
    if (!m_running.exchange(false))
    {
        return;
    }

    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        PostQueuedCompletionStatus(m_iocp, 0, 0, nullptr);
    }

    for (HANDLE thread : m_threads)
    {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    m_threads.clear();

    if (m_iocp != nullptr)
    {
        CloseHandle(m_iocp);
        m_iocp = nullptr;
    }
}

bool IocpCore::AssociateHandle(HANDLE handle, ULONG_PTR key)
{
    if (m_iocp == nullptr)
    {
        return false;
    }
    return CreateIoCompletionPort(handle, m_iocp, key, 0) != nullptr;
}

bool IocpCore::PostStatus(DWORD bytes, ULONG_PTR key, OVERLAPPED* overlapped)
{
    if (m_iocp == nullptr)
    {
        return false;
    }
    return PostQueuedCompletionStatus(m_iocp, bytes, key, overlapped) != FALSE;
}

DWORD WINAPI IocpCore::WorkerThunk(LPVOID param)
{
    auto* self = static_cast<IocpCore*>(param);
    self->WorkerLoop();
    return 0;
}

void IocpCore::WorkerLoop()
{
    while (m_running.load())
    {
        DWORD bytes = 0;
        ULONG_PTR key = 0;
        OVERLAPPED* overlapped = nullptr;
        BOOL ok = GetQueuedCompletionStatus(m_iocp, &bytes, &key, &overlapped, INFINITE);

        if (!m_running.load())
        {
            break;
        }

        if (overlapped == nullptr)
        {
            continue;
        }

        if (!ok)
        {
            continue;
        }
    }
}
