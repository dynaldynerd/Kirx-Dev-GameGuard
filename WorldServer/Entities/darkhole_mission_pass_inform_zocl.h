#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_mission_pass_inform_zocl{ char szCompleteMsgCode[17]; unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _darkhole_mission_pass_inform_zocl::size() const{ return 17;}