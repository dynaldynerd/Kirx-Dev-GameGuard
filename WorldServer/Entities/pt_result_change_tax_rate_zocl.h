#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _pt_result_change_tax_rate_zocl{ unsigned __int8 byRet; unsigned __int8 byNextTax; _pt_result_change_tax_rate_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _pt_result_change_tax_rate_zocl::_pt_result_change_tax_rate_zocl(){ memset_0(this, 0, sizeof(*this));}inline __int64 _pt_result_change_tax_rate_zocl::size() const{ return 2;}