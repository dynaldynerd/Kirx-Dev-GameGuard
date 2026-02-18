#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _sf_delay_download_result_zocl{ struct  _eff_list { unsigned __int8 byEffectCode; unsigned __int16 wEffectIndex; unsigned int dwRemainTime; _eff_list(); }; struct  _mas_list { unsigned __int8 byEffectCode; unsigned __int8 byMastery; unsigned int dwRemainTime; _mas_list(); }; _eff_list EFF[10]; _mas_list MAS[10]; _sf_delay_download_result_zocl();};
#pragma pack(pop)
inline _sf_delay_download_result_zocl::_eff_list::_eff_list(){ byEffectCode = static_cast<unsigned __int8>(-1);}inline _sf_delay_download_result_zocl::_mas_list::_mas_list(){ byEffectCode = static_cast<unsigned __int8>(-1);}inline _sf_delay_download_result_zocl::_sf_delay_download_result_zocl() = default;