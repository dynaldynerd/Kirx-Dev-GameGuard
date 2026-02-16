#pragma once

#pragma pack(push, 1)
  struct FixMsg
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int8 byDur;
    unsigned __int16 wIndex;
    unsigned int dwOwnerSerial;
    unsigned __int16 posShort[3];
    unsigned __int8 byStateCode;
    unsigned __int8 byThrowerRaceCode;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct StateMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwOwnerSerial;
    unsigned __int8 byStateCode;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct CreateMsg
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    unsigned __int8 byDur;
    unsigned __int16 wIndex;
    unsigned int dwOwnerSerial;
    unsigned __int8 byThrowerID;
    unsigned __int16 wThrowerIndex;
    unsigned int dwThrowerSerial;
    unsigned __int8 byStateCode;
    unsigned __int16 posShort[3];
    unsigned __int8 byThrowerRaceCode;
  };
#pragma pack(pop)

