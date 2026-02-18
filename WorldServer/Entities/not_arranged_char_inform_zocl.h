#pragma once
#include "IdaCompat.h"
#include "DqsDbStructs.h"
#pragma pack(push, 1)
struct  _not_arranged_char_inform_zocl{ unsigned __int8 byCharNum; _NOT_ARRANGED_AVATOR_DB CharList[50]; _not_arranged_char_inform_zocl(); __int64 size();};
#pragma pack(pop)
inline _not_arranged_char_inform_zocl::_not_arranged_char_inform_zocl() : byCharNum(0){}inline __int64 _not_arranged_char_inform_zocl::size(){ if (byCharNum > 0x32u) { byCharNum = 0; } return 3451 - 69LL * (50 - byCharNum);}