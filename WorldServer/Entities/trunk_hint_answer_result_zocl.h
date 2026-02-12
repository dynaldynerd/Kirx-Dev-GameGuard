#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _trunk_hint_answer_result_zocl
{
  unsigned __int8 byRetCode;
  char wszPassword[13];

  _trunk_hint_answer_result_zocl();
};
#pragma pack(pop)

inline _trunk_hint_answer_result_zocl::_trunk_hint_answer_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
