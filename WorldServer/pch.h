#pragma once

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
