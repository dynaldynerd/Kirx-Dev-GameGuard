#pragma once

#pragma pack(push, 1)
  struct GuardTowerCreateMsg
  {
    unsigned __int16 wIndex;
    unsigned __int16 wRecordIndex;
    unsigned int dwObjSerial;
    short pos[3];
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct GuardTowerDestroyMsg
  {
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    unsigned __int8 byDesType;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct GuardTowerFixPositionMsg
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    __int16 pos[3];
    __int16 nBuildSec;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct GuardTowerCompleteMsg
  {
    unsigned int dwTowerSerial;
    unsigned int dwMasterSerial;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct GuardTowerAttackMsg
  {
    unsigned int dwTowerSerial;
    unsigned __int8 byPart;
    unsigned __int8 byCritical;
    unsigned __int8 byTargetId;
    unsigned int dwTargetSerial;
    __int16 nDamage;
  };
#pragma pack(pop)

