#pragma once

#include "IdaCompat.h"
#include "notify_race_leader_s_owner_u_taxrate.h"

/* 1807 */
class  CNotifyNotifyRaceLeaderSownerUTaxrate
{
public:
  CNotifyNotifyRaceLeaderSownerUTaxrate();
  void Init();
  void UpdateTaxRate(unsigned __int8 byRace, unsigned __int8 byTaxRate);
  void UpdateRaceLeader(unsigned __int8 byRace, unsigned __int8 byNth, char *wszLeaderName);
  void UpdateSettlementOwner(unsigned __int8 byRace, unsigned int dw1ThGuildSerial, unsigned int dw2ThGuildSerial);
  void Notify(unsigned __int8 byRace);
  void Notify(unsigned __int8 byRace, unsigned __int16 wIndex);

  _notify_race_leader_s_owner_u_taxrate m_Send[3];
};

