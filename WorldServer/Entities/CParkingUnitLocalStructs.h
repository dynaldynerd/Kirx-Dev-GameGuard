#pragma once

#pragma pack(push, 1)
  struct DestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwSerial;
    unsigned __int8 byDestroyType;
  };
#pragma pack(pop)

