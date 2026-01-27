#pragma once

#include "IdaCompat.h"

/* 1331 */
struct IDA_GUID
{
  unsigned int Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
};

struct IDA_IUnknown;

/* 1333 */
struct /*VFT*/ IDA_IUnknown_vtbl
{
  HRESULT (__fastcall *QueryInterface)(IDA_IUnknown *pThis, const struct IDA_GUID *, void **);
  unsigned int (__fastcall *AddRef)(IDA_IUnknown *pThis);
  unsigned int (__fastcall *Release)(IDA_IUnknown *pThis);
};

/* 1332 */
struct __cppobj IDA_IUnknown
{
  IDA_IUnknown_vtbl *__vftable /*VFT*/;
};

/* 1357 */
struct __cppobj IDirect3DResource8 : IDA_IUnknown
{
};

/* 1367 */
struct __cppobj IDirect3DVertexBuffer8 : IDirect3DResource8
{
};

/* 1368 */
struct __cppobj IDirect3DIndexBuffer8 : IDirect3DResource8
{
};

/* 1385 */
union $B3185CC329BDC5A6DA4B2C96D7DC3DD4
{
  float x;
  float pos[1];
};

/* 1386 */
union $25C8602110B22A2EB54BB3EE6752756B
{
  float nx;
  float normal[1];
};

/* 1387 */
struct _D3DR3VERTEX_TEX1
{
  $B3185CC329BDC5A6DA4B2C96D7DC3DD4 ___u0;
  float y;
  float z;
  $25C8602110B22A2EB54BB3EE6752756B ___u3;
  float ny;
  float nz;
  unsigned int color;
  float u;
  float v;
};

/* 1388 */
union $42A39AC029B3127F486DF502FF8A7E09
{
  float x;
  float local[1];
};

/* 1389 */
struct _D3DR3VERTEX_TEX2
{
  $42A39AC029B3127F486DF502FF8A7E09 ___u0;
  float y;
  float z;
  $25C8602110B22A2EB54BB3EE6752756B ___u3;
  float ny;
  float nz;
  unsigned int color;
  float uv[2][2];
};
