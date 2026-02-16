#pragma once

struct Entry
{
  unsigned __int8 byTicketType;
  unsigned __int16 wTicketNum;
};

#pragma pack(push, 1)
struct TicketNumMsg
{
  unsigned int dwObjSerial;
  Entry entry[2];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MerchantMoveMsg
{
  unsigned int dwObjSerial;
  __int16 pos[3];
  __int16 nTarX;
  __int16 nTarZ;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MerchantDestroyMsg
{
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MerchantRealMoveMsg
{
  unsigned __int16 wRecordIndex;
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
  __int16 pos[3];
  __int16 nTarX;
  __int16 nTarZ;
  unsigned __int16 wLastContEffect;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct MerchantFixPosMsg
{
  unsigned __int16 wRecordIndex;
  unsigned __int16 wIndex;
  unsigned int dwObjSerial;
  __int16 pos[3];
  unsigned __int16 wLastContEffect;
};
#pragma pack(pop)

