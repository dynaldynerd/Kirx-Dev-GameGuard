#pragma once

struct _log_sheet_usernum
{
  int nAveragePerHour;
  int nMaxPerHour;
  int nCount;

  int size() const
  {
    return sizeof(*this);
  }
};

