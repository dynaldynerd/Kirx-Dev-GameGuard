#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _guild_alter_member_grade_inform_zocl{ struct __list { unsigned int dwMemberSerial; unsigned __int8 byRank; unsigned __int8 byGrade; }; unsigned __int8 byAlterMemberNum; __list MemberList[50]; _guild_alter_member_grade_inform_zocl(); __int64 size();};
#pragma pack(pop)
