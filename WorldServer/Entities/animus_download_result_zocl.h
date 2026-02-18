#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _animus_download_result_zocl{ struct _list { char sItemIndex; unsigned __int64 dwExp; unsigned int dwParam; unsigned __int8 byCsMethod; unsigned int dwT; }; unsigned __int8 byAnimusNum; _list AnimusList[4]; _animus_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
