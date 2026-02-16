#pragma once

    struct _limit_run_result
    {
      char byRet;
      unsigned __int8 data[7];
    };

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

