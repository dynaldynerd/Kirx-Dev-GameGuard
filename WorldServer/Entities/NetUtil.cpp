#include "pch.h"

#include "NetUtil.h"

unsigned char byLocalIP[4]{};

unsigned int GetIPAddress()
{
  char szAddr[80]{};
  if (GetIPAddress(szAddr))
  {
    return inet_addr(szAddr);
  }

  return static_cast<unsigned int>(-1);
}

__int64 GetIPAddress(char *szAddr)
{
  char name[264]{};
  gethostname(name, 255);
  hostent *host = gethostbyname(name);
  if (!host || !host->h_addr_list || !*host->h_addr_list)
  {
    return 0;
  }

  in_addr addr{};
  std::memcpy(&addr, *reinterpret_cast<void **>(host->h_addr_list), sizeof(addr));
  const char *src = inet_ntoa(addr);
  if (src)
  {
    std::memcpy(szAddr, src, 16);
    return 1;
  }

  return 0;
}

__int64 GetIPAddress(sockaddr_in *pAddr)
{
  char name[264]{};
  gethostname(name, 255);
  hostent *host = gethostbyname(name);
  if (!host || !host->h_addr_list || !*host->h_addr_list)
  {
    return 0;
  }

  std::memcpy(pAddr, *reinterpret_cast<void **>(host->h_addr_list), 4u);
  return 1;
}
