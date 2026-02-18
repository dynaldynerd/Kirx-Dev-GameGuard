#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _post_content_result_zocl{ struct _post_item { unsigned __int8 byTableCode; unsigned __int16 wItemIndex; unsigned __int64 dwDur; unsigned int dwLv; }; unsigned __int8 byErrCode; unsigned int dwPostSerial; char wszContent[201]; _post_item Item; unsigned int dwGold; _post_content_result_zocl(); unsigned __int16 size() const;};
#pragma pack(pop)
inline _post_content_result_zocl::_post_content_result_zocl(){ memset_0(this, 0, sizeof(_post_content_result_zocl));}inline unsigned __int16 _post_content_result_zocl::size() const{ return static_cast<unsigned __int16>(sizeof(_post_content_result_zocl));}