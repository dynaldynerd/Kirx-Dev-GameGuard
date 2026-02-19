#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include "framework.h"

#ifndef __cppobj
#define __cppobj
#endif

#ifndef __unaligned
#define __unaligned
#endif

#ifndef __bitmask
#define __bitmask
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include <atomic>
#include <string>
#include <vector>

#include "Entities/CABC75A24E330D1CBB803A094EF8.h"
#include "Entities/CMainThread.h"
#include "Entities/WorldServerUtil.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
