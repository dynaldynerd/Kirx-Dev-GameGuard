#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _pt_inform_appoint_zocl{ unsigned __int8 byClassType; char wszAvatorName[17]; _pt_inform_appoint_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _pt_inform_appoint_zocl::_pt_inform_appoint_zocl(){ memset_0(this, 0, sizeof(*this));}inline __int64 _pt_inform_appoint_zocl::size() const{ return 18;}