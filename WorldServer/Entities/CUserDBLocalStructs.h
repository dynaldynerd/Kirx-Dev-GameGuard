#pragma once

  struct _billing_msg
  {
    __int16 iType;
    int lRemainTime;
    _SYSTEMTIME stEndDate;
  };

#pragma pack(push, 1)
  struct FireguardBlockRequest
  {
    char szAccountID[13];
    unsigned int dwAccountSerial;
    unsigned int dwIP;
  };
#pragma pack(pop)

