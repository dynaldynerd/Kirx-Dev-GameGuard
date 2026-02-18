#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_channel_close_inform_zocl{ char sDum; unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _darkhole_channel_close_inform_zocl::size() const{ return 1;}