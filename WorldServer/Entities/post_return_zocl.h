#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _post_return_zocl{ struct _post_item { unsigned __int8 byTableCode; unsigned __int16 wItemIndex; unsigned __int64 dwDur; unsigned int dwLv; }; unsigned __int8 byErrCode; unsigned int dwPostSerial; char wszRecvName[17]; char wszTitle[21]; char wszContent[201]; _post_item Item; unsigned int dwGold; unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _post_return_zocl::size() const{ return static_cast<unsigned __int16>(sizeof(_post_return_zocl));}