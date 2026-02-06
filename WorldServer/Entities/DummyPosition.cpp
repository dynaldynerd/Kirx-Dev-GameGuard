#include "pch.h"

#include "DummyPosition.h"

void _dummy_position::SetActiveMonNum(__int16 nAlter)
{
  m_wActiveMon = static_cast<unsigned __int16>(m_wActiveMon + nAlter);
}
