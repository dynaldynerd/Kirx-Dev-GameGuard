#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _trunk_pw_hint_index_result_zocl
{
  unsigned __int8 byRetCode;
  unsigned __int8 byHintIndex;

  _trunk_pw_hint_index_result_zocl();
};
#pragma pack(pop)

inline _trunk_pw_hint_index_result_zocl::_trunk_pw_hint_index_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
