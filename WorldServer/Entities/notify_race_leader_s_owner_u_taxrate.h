#pragma once

#include "IdaCompat.h"

struct _notify_race_leader_s_owner_u_taxrate
{
  char wszRaceLeaderName[9][17];
  char wszSettlement1OwnerGuildName[17];
  char wszSettlement1OwnerGuildMasterName[17];
  char wszSettlement2OwnerGuildName[17];
  char wszSettlement2OwnerGuildMasterName[17];
  unsigned __int8 byTaxRate;
};
