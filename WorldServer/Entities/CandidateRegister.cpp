#include "pch.h"

#include "CandidateRegister.h"

#include <cstring>

CandidateRegister::CandidateRegister()
  : ElectProcessor(_eCandidateRegister)
{
  memset(m_kSend, 0, sizeof(m_kSend));
  m_byPtType[0] = 56;
  m_byPtType[1] = 2;
}
