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
struct  IDA_IUnknown
{
  IDA_IUnknown_vtbl *__vftable /*VFT*/;
};

/* 1357 */
struct  IDirect3DResource8 : IDA_IUnknown
{
};

struct IDirect3DDevice8;

/* 1367 */
struct  IDirect3DVertexBuffer8 : IDirect3DResource8
{
  HRESULT (__fastcall *Lock)(IDirect3DVertexBuffer8 *self, unsigned int offset, unsigned int size, unsigned char **data, unsigned int flags);
  HRESULT (__fastcall *Unlock)(IDirect3DVertexBuffer8 *self);
  unsigned int (__fastcall *Release)(IDirect3DVertexBuffer8 *self);
  unsigned char *m_Data;
  unsigned int m_Size;
};

/* 1368 */
struct  IDirect3DIndexBuffer8 : IDirect3DResource8
{
  HRESULT (__fastcall *Lock)(IDirect3DIndexBuffer8 *self, unsigned int offset, unsigned int size, unsigned char **data, unsigned int flags);
  HRESULT (__fastcall *Unlock)(IDirect3DIndexBuffer8 *self);
  unsigned int (__fastcall *Release)(IDirect3DIndexBuffer8 *self);
  unsigned char *m_Data;
  unsigned int m_Size;
};

struct  IDirect3DTexture8 : IDirect3DResource8
{
  unsigned char *m_Data;
  unsigned int m_Size;
};

/* Minimal device interface used by decompiled code */
struct  IDirect3DDevice8
{
  HRESULT (__fastcall *CreateVertexBuffer)(
    IDirect3DDevice8 *self,
    unsigned int size,
    unsigned int usage,
    unsigned int fvf,
    unsigned int pool,
    IDirect3DVertexBuffer8 **out);
  HRESULT (__fastcall *CreateIndexBuffer)(IDirect3DDevice8 *self, unsigned int size, unsigned int usage);
};

#ifndef D3DPOOL_MANAGED
#define D3DPOOL_MANAGED 1
#endif

#if !defined(_d3d9TYPES_H_) && !defined(_D3D9TYPES_H_) && !defined(_D3D9_H_) && !defined(_d3d8_h_) && !defined(_D3D8_H_) && !defined(_D3D8TYPES_H_)
typedef unsigned int D3DFORMAT;
#endif
#ifndef D3DFMT_R5G6B5
#define D3DFMT_R5G6B5 23
#endif
#ifndef D3DFMT_A8R8G8B8
#define D3DFMT_A8R8G8B8 21
#endif

/* Minimal DDS header for mipmap calculations */
struct _DDSURFACEDESC2
{
  unsigned int dwSize;
  unsigned int dwFlags;
  unsigned int dwHeight;
  unsigned int dwWidth;
  int lPitch;
  unsigned int dwDepth;
  unsigned int dwMipMapCount;
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
  union
  {
    float x;
    float pos[1];
  };
  float y;
  float z;
  union
  {
    float nx;
    float normal[1];
  };
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
