#pragma once

#include "IdaCompat.h"
#include <winsock2.h>

extern unsigned char byLocalIP[4];

unsigned int GetIPAddress();
__int64 GetIPAddress(char *szAddr);
__int64 GetIPAddress(sockaddr_in *pAddr);
