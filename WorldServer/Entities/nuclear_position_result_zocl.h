#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _nuclear_position_result_zocl{ unsigned int dwMasterSerial; float zPos[3]; unsigned __int8 byUseClass; _nuclear_position_result_zocl() { dwMasterSerial = 0; zPos[0] = 0.0f; zPos[1] = 0.0f; zPos[2] = 0.0f; byUseClass = 0; }};
#pragma pack(pop)
