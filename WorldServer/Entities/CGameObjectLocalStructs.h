#pragma once

#pragma pack(push, 1)
  struct BreakStopMsg
  {
    unsigned __int8 byID;
    unsigned int dwObjSerial;
    __int16 pos[3];
  };
#pragma pack(pop)

