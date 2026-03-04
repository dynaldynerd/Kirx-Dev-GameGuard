#pragma once

#include "IdaCompat.h"
#include "D3DXMATRIX.h"
#include <d3d.h>

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
struct IDA_IUnknown
{
  IDA_IUnknown_vtbl *__vftable /*VFT*/;
};

struct D3DMATERIAL8
{
  D3DCOLORVALUE Diffuse;
  D3DCOLORVALUE Ambient;
  D3DCOLORVALUE Specular;
  D3DCOLORVALUE Emissive;
  float Power;
};

struct D3DLIGHT8
{
  int Type;
  D3DCOLORVALUE Diffuse;
  D3DCOLORVALUE Specular;
  D3DCOLORVALUE Ambient;
  D3DVECTOR Position;
  D3DVECTOR Direction;
  float Range;
  float Falloff;
  float Attenuation0;
  float Attenuation1;
  float Attenuation2;
  float Theta;
  float Phi;
};

/* 1357 */
struct IDirect3DResource8 : IDA_IUnknown
{
};

struct IDirect3DBaseTexture8 : IDirect3DResource8
{
};

struct IDirect3DDevice8;

/* 1367 */
struct IDirect3DVertexBuffer8 : IDirect3DResource8
{
  HRESULT (__fastcall *Lock)(IDirect3DVertexBuffer8 *self, unsigned int offset, unsigned int size, unsigned char **data, unsigned int flags);
  HRESULT (__fastcall *Unlock)(IDirect3DVertexBuffer8 *self);
  unsigned int (__fastcall *Release)(IDirect3DVertexBuffer8 *self);
  unsigned char *m_Data;
  unsigned int m_Size;
};

/* 1368 */
struct IDirect3DIndexBuffer8 : IDirect3DResource8
{
  HRESULT (__fastcall *Lock)(IDirect3DIndexBuffer8 *self, unsigned int offset, unsigned int size, unsigned char **data, unsigned int flags);
  HRESULT (__fastcall *Unlock)(IDirect3DIndexBuffer8 *self);
  unsigned int (__fastcall *Release)(IDirect3DIndexBuffer8 *self);
  unsigned char *m_Data;
  unsigned int m_Size;
};

struct IDirect3DTexture8 : IDirect3DBaseTexture8
{
  unsigned char *m_Data;
  unsigned int m_Size;
};

/* Device function surface used by decompiled call style */
struct IDirect3DDevice8
{
  HRESULT (__fastcall *CreateVertexBuffer)(
    IDirect3DDevice8 *self,
    unsigned int size,
    unsigned int usage,
    unsigned int fvf,
    _D3DPOOL pool,
    IDirect3DVertexBuffer8 **out);
  HRESULT (__fastcall *CreateIndexBuffer)(
    IDirect3DDevice8 *self,
    unsigned int size,
    unsigned int usage,
    _D3DFORMAT format,
    _D3DPOOL pool,
    IDirect3DIndexBuffer8 **out);
  HRESULT (__fastcall *SetTransform)(IDirect3DDevice8 *self, _D3DTRANSFORMSTATETYPE state, const _D3DMATRIX *matrix);
  HRESULT (__fastcall *SetRenderState)(IDirect3DDevice8 *self, _D3DRENDERSTATETYPE state, unsigned int value);
  HRESULT (__fastcall *SetTexture)(IDirect3DDevice8 *self, unsigned int stage, IDirect3DBaseTexture8 *texture);
  HRESULT (__fastcall *SetTextureStageState)(IDirect3DDevice8 *self, unsigned int stage, _D3DTEXTURESTAGESTATETYPE type, unsigned int value);
  HRESULT (__fastcall *DrawIndexedPrimitive)(
    IDirect3DDevice8 *self,
    _D3DPRIMITIVETYPE primitiveType,
    unsigned int minIndex,
    unsigned int numVertices,
    unsigned int startIndex,
    unsigned int primitiveCount);
  HRESULT (__fastcall *DrawPrimitive)(
    IDirect3DDevice8 *self,
    _D3DPRIMITIVETYPE primitiveType,
    unsigned int startVertex,
    unsigned int primitiveCount);
  HRESULT (__fastcall *DrawPrimitiveUP)(
    IDirect3DDevice8 *self,
    _D3DPRIMITIVETYPE primitiveType,
    unsigned int primitiveCount,
    const void *data,
    unsigned int stride);
  HRESULT (__fastcall *DrawIndexedPrimitiveUP)(
    IDirect3DDevice8 *self,
    _D3DPRIMITIVETYPE primitiveType,
    unsigned int minVertexIndex,
    unsigned int numVertices,
    unsigned int primitiveCount,
    const void *indexData,
    _D3DFORMAT indexDataFormat,
    const void *vertexData,
    unsigned int vertexStreamZeroStride);
  HRESULT (__fastcall *SetVertexShader)(IDirect3DDevice8 *self, unsigned int shader);
  HRESULT (__fastcall *SetVertexShaderConstant)(IDirect3DDevice8 *self, unsigned int reg, const void *data, unsigned int count);
  HRESULT (__fastcall *SetStreamSource)(IDirect3DDevice8 *self, unsigned int stream, IDirect3DVertexBuffer8 *streamData, unsigned int stride);
  HRESULT (__fastcall *SetIndices)(IDirect3DDevice8 *self, IDirect3DIndexBuffer8 *indexData, unsigned int baseVertexIndex);
  HRESULT (__fastcall *LightEnable)(IDirect3DDevice8 *self, unsigned int index, int enable);
  HRESULT (__fastcall *SetLight)(IDirect3DDevice8 *self, unsigned int index, const void *light);
  HRESULT (__fastcall *SetMaterial)(IDirect3DDevice8 *self, const void *material);
};

/* Minimal DDS header for mipmap calculations */
struct IDA_DDSURFACEDESC2
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

struct _D3DR3VERTEX_TEX0
{
  float x;
  float y;
  float z;
  unsigned int color;
};
