#pragma once

#pragma pack(push, 1)
  struct TrapFixPosMsg
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    __int16 pos[3];
    __int16 nBuildSec;
    unsigned int dwMasterSerial;
    unsigned __int8 byNotBreakTranspar;
    unsigned __int8 byRaceCode;
  };
#pragma pack(pop)

