#pragma once

#include "IdaCompat.h"

struct __cppobj _guild_vote_process_inform_zocl
{
  bool bStart;
  unsigned __int8 byMatterType;
  __unaligned __declspec(align(1)) unsigned int dwMatterDst;
  __unaligned __declspec(align(1)) unsigned int dwMatterObj1;
  __unaligned __declspec(align(1)) unsigned int dwMatterObj2;
  __unaligned __declspec(align(1)) unsigned int dwMatterObj3;
  __unaligned __declspec(align(1)) unsigned int dwMatterVoteSynKey;
  __unaligned __declspec(align(1)) unsigned int dwSuggesterSerial;
  unsigned __int8 byLoginSeniorNum;
  unsigned __int8 byTotalSeniorNum;
  bool bActed;
  unsigned __int8 byApprPoint;
  unsigned __int8 byOppoPoint;
  char wszDestGuildName[17];
  unsigned __int8 byDestGuildGrade;
  unsigned __int8 byDestGuildRace;
  unsigned __int8 byCommentLen;
  char wszComment[65];

  _guild_vote_process_inform_zocl();
  __int64 size();
};
