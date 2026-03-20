#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"
#include "Packet/ZoneClientPacket.h"

class  CandidateRegister : public ElectProcessor
{
public:
  CandidateRegister();
  ~CandidateRegister() override;
  bool Initialize() override;
  int Doit(Cmd eCmd, CPlayer *pOne, char *pdata) override;

  unsigned __int8 m_byPtType[2];
  _pt_result_fcandidacy_list_zocl m_kSend[3];
  bool m_bInitCandidate;

private:
  int _CheckPlayerInfo(CPlayer *pOne);
  bool _AddToPacket(CPlayer *pOne, unsigned int dwWinCnt);
  void _SortCandidacyByPvpPoint(unsigned __int8 byRace);
  int _SendList(unsigned __int16 wSock, unsigned __int8 byRace);
  int _Regist(CPlayer *pOne, char *pdata);
  void _InitCandidate();
  void _UpdatePacketWin(unsigned __int8 byRace, char *wszName, unsigned int dwWinCnt);
};
