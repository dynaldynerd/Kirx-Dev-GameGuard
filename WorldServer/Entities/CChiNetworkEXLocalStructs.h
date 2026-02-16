#pragma once

#pragma pack(push, 1)
  struct ApexBlockRequest
  {
    char szAccountID[13];
    unsigned int dwAccountSerial;
    unsigned int dwIP;
  };
#pragma pack(pop)

