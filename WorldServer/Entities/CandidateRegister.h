#pragma once

#include "IdaCompat.h"
#include "ElectProcessor.h"

class __cppobj CandidateRegister : public ElectProcessor
{
public:
  struct __cppobj _pt_result_fcandidacy_list_zocl
  {
    struct __candi_info
    {
      unsigned __int8 byGrade;
      long double dPvpPoint;
      unsigned int dwWinCnt;
      char wszAvatorName[17];
      char wszGuildName[17];
    };

    unsigned __int8 byCnt;
    __candi_info Candidacy[500];
  };

  unsigned __int8 m_byPtType[2];
  _pt_result_fcandidacy_list_zocl m_kSend[3];
  bool m_bInitCandidate;
};
