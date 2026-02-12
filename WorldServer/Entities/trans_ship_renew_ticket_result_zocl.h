#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _trans_ship_renew_ticket_result_zocl
{
  unsigned __int8 byErrCode;

  _trans_ship_renew_ticket_result_zocl();
};
#pragma pack(pop)

inline _trans_ship_renew_ticket_result_zocl::_trans_ship_renew_ticket_result_zocl()
{
  memset_0(this, 0, sizeof(*this));
}
