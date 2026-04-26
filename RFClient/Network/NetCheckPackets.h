#pragma once

#include <windows.h>

#include "Protocol.h"

struct _check_query
{
  char sDum;

  int size() const;
};

struct _check_answer
{
  char sDum;
};

struct _check_speed_hack_ask
{
  DWORD dwSrcKey[CHECK_KEY_NUM];
};

struct _check_speed_hack_ans
{
  DWORD dwKey[CHECK_KEY_NUM];

  int size() const;
};

DWORD *CalcCodeKey(DWORD *pdwCode);
