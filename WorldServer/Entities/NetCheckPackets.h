#pragma once

#include "IdaCompat.h"

struct _check_query
{
  char sDum;

  unsigned __int64 size() const;
};

struct _check_answer
{
  char sDum;
};

struct _ping_pong
{
  char sDum[111];
};

struct _check_speed_hack_ask
{
  unsigned int dwSrcKey[4];
};

struct _check_speed_hack_ans
{
  unsigned int dwKey[4];

  unsigned __int64 size() const;
};

unsigned int *CalcCodeKey(unsigned int *pdwCode);
