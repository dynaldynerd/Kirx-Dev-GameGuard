#pragma once

#include "IdaCompat.h"

struct D3DXMATRIX
{
  union
  {
    struct
    {
      float _11;
      float _12;
      float _13;
      float _14;
      float _21;
      float _22;
      float _23;
      float _24;
      float _31;
      float _32;
      float _33;
      float _34;
      float _41;
      float _42;
      float _43;
      float _44;
    };
    float m[4][4];
  };
};
