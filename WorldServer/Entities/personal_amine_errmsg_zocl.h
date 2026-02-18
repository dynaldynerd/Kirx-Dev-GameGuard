#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _personal_amine_errmsg_zocl{ unsigned __int8 byErrCode; _personal_amine_errmsg_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _personal_amine_errmsg_zocl::_personal_amine_errmsg_zocl(){ byErrCode = static_cast<unsigned __int8>(-1);}inline __int64 _personal_amine_errmsg_zocl::size() const{ return 1;}