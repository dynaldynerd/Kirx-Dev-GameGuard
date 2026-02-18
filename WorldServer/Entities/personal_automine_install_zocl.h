#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _personal_automine_install_zocl{ unsigned int dwObjSerial; unsigned __int16 wObjIndex; unsigned int dwOwnerSerial; unsigned __int16 wItemSerial; unsigned __int16 wItemTblIndex; float fPos[3]; unsigned __int8 byFilledSlotCnt; _personal_automine_install_zocl() { memset_0(this, 0, sizeof(*this)); } unsigned int size() const { return sizeof(*this); }};
#pragma pack(pop)
