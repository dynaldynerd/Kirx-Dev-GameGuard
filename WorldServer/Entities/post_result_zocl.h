#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _post_result_zocl{ unsigned __int8 byErrCode; unsigned int dwGold; _post_result_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _post_result_zocl::_post_result_zocl() : byErrCode(0), dwGold(0){}inline __int64 _post_result_zocl::size() const{ return 5;}