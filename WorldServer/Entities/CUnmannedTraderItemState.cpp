#include "pch.h"

#include "CUnmannedTraderItemState.h"

CUnmannedTraderItemState::CUnmannedTraderItemState()
  : m_eState(STATE::NONE)
{
  Clear();
}

void CUnmannedTraderItemState::Clear()
{
  this->m_eState = STATE::NONE;
}
