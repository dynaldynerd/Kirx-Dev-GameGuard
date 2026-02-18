#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"

class  CandidateRegister : public ElectProcessor
{
public:
#pragma pack(push, 1)
  struct  _pt_result_fcandidacy_list_zocl
  {
    #pragma pack(push, 1)
    struct __candi_info
    {
      unsigned __int8 byGrade;
      long double dPvpPoint;
      unsigned int dwWinCnt;
      char wszAvatorName[17];
      char wszGuildName[17];
    };
    #pragma pack(pop)

    _pt_result_fcandidacy_list_zocl();
    unsigned __int16 size() const;

    unsigned __int8 byCnt;
    __candi_info Candidacy[500];
  };
#pragma pack(pop)

  CandidateRegister();
  ~CandidateRegister() override;
  bool Initialize() override;
  int Doit(Cmd eCmd, CPlayer *pOne, char *pdata) override;

  unsigned __int8 m_byPtType[2];
  _pt_result_fcandidacy_list_zocl m_kSend[3];
  bool m_bInitCandidate;

private:
  unsigned int _CheckPlayerInfo(CPlayer *pOne);
  bool _AddToPacket(CPlayer *pOne, unsigned int dwWinCnt);
  void _SortCandidacyByPvpPoint(unsigned __int8 byRace);
  int _SendList(unsigned __int16 wSock, unsigned __int8 byRace);
  int _Regist(CPlayer *pOne, char *pdata);
  void _InitCandidate();
  void _UpdatePacketWin(unsigned __int8 byRace, char *wszName, unsigned int dwWinCnt);
};
