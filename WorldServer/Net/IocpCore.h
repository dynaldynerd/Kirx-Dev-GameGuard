#pragma once

#include <windows.h>
#include <atomic>
#include <vector>

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    bool Start(unsigned int workerCount);
    void Stop();

    bool AssociateHandle(HANDLE handle, ULONG_PTR key);
    bool PostStatus(DWORD bytes, ULONG_PTR key, OVERLAPPED* overlapped);

private:
    static DWORD WINAPI WorkerThunk(LPVOID param);
    void WorkerLoop();

    HANDLE m_iocp;
    std::vector<HANDLE> m_threads;
    std::atomic<bool> m_running;
};
