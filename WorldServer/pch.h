#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef WINVER
#define WINVER 2560
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 2560
#endif

#include "framework.h"

#ifndef __bitmask
#define __bitmask
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>
#include <string>
#include <vector>

#include "Entities/GlobalObjectDefs.h"
#include "Entities/CMainThread.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
