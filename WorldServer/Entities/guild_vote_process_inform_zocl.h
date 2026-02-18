#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _guild_vote_process_inform_zocl{ bool bStart; unsigned __int8 byMatterType; unsigned int dwMatterDst; unsigned int dwMatterObj1; unsigned int dwMatterObj2; unsigned int dwMatterObj3; unsigned int dwMatterVoteSynKey; unsigned int dwSuggesterSerial; unsigned __int8 byLoginSeniorNum; unsigned __int8 byTotalSeniorNum; bool bActed; unsigned __int8 byApprPoint; unsigned __int8 byOppoPoint; char wszDestGuildName[17]; unsigned __int8 byDestGuildGrade; unsigned __int8 byDestGuildRace; unsigned __int8 byCommentLen; char wszComment[65]; _guild_vote_process_inform_zocl(); __int64 size();};
#pragma pack(pop)
