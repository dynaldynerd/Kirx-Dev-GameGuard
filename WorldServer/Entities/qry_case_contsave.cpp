#include "pch.h"

#include "qry_case_contsave.h"

_qry_case_contsave::_qry_case_contsave()
  = default;

_qry_case_contsave::~_qry_case_contsave() = default;

int _qry_case_contsave::size()
{
  return static_cast<int>(sizeof(*this));
}
