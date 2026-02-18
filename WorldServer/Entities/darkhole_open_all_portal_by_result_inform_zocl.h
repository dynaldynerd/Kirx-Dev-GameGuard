#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_open_all_portal_by_result_inform_zocl{ unsigned __int8 byCnt; unsigned __int8 byPotalIndex[128]; _darkhole_open_all_portal_by_result_inform_zocl() : byCnt(0), byPotalIndex{} { } unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _darkhole_open_all_portal_by_result_inform_zocl::size() const{ return static_cast<unsigned __int16>(byCnt + 1);}