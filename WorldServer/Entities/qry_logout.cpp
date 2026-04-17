#include "pch.h"

#include "qry_logout.h"

_qry_sheet_logout::_qry_sheet_logout() = default;

_qry_sheet_logout::~_qry_sheet_logout() = default;

int _qry_sheet_logout::size()
{
  return static_cast<int>(sizeof(*this));
}
