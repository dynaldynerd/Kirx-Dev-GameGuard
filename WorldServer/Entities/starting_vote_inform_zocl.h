#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _starting_vote_inform_zocl{ int nVoteSerial; unsigned __int8 byLimGrade; unsigned __int16 wLeftSec; unsigned __int16 wContentSize; char wszContent[1280]; _starting_vote_inform_zocl(); __int64 size();};
#pragma pack(pop)
