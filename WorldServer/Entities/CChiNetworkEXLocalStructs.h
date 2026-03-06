#pragma once

#pragma pack(push, 1)
struct ApexMessageHeader
{
  unsigned int dwSize;
  unsigned __int8 byType;
  unsigned int dwSID;
};
#pragma pack(pop)

#pragma pack(push, 1)
  struct ApexBlockRequest
  {
    char szAccountID[13];
    unsigned int dwAccountSerial;
    unsigned int dwIP;
  };
#pragma pack(pop)

