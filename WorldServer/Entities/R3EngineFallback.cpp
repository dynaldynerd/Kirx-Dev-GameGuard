#include "pch.h"

#include "WorldServerUtil.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "CBsp.h"
#include "CEntity.h"
#include "CIndexBuffer.h"
#include "CParticle.h"
#include "CVertexBuffer.h"
#include "GlobalObjects.h"
#include "R3EngineGlobals.h"
#include "R3EngineState.h"

namespace
{
struct LineVertex
{
  float x;
  float y;
  float z;
  unsigned int color;
};

struct DynamicLightEntry
{
  bool used;
  float pos[3];
  float diameter;
  unsigned int color;
  IDirect3DBaseTexture8 *texture;
  unsigned int blendType;
  bool magic;
};

DynamicLightEntry g_dynamicLights[16]{};
unsigned int g_dynamicLightCount = 0;
float g_multiLayerCamera[3]{};
float g_multiLayerTime = 0.0f;
int g_reflectionState = 0;

void NormalizePlane(float *plane)
{
  const float len = std::sqrt((plane[0] * plane[0]) + (plane[1] * plane[1]) + (plane[2] * plane[2]));
  if (len <= 0.0f)
  {
    return;
  }
  plane[0] /= len;
  plane[1] /= len;
  plane[2] /= len;
  plane[3] /= len;
}
} // namespace

void GetCameraPos(float (*a1)[3])
{
  if (!a1)
  {
    return;
  }

  (*a1)[0] = stState.mInvMatView[3][0];
  (*a1)[1] = stState.mInvMatView[3][1];
  (*a1)[2] = stState.mInvMatView[3][2];
}

void SetVPIPTex1(IDirect3DVertexBuffer8 *vertexBuffer, IDirect3DIndexBuffer8 *indexBuffer)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }

  device->SetVertexShader(device, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
  device->SetStreamSource(device, 0, vertexBuffer, sizeof(_D3DR3VERTEX_TEX1));
  device->SetIndices(device, indexBuffer, 0);
}

void SetVPIPTex2(IDirect3DVertexBuffer8 *vertexBuffer, IDirect3DIndexBuffer8 *indexBuffer)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }

  device->SetVertexShader(device, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);
  device->SetStreamSource(device, 0, vertexBuffer, sizeof(_D3DR3VERTEX_TEX2));
  device->SetIndices(device, indexBuffer, 0);
}

void ResetVertexBufferCache()
{
  // this is not a stub
}

void SetProjectShadow(float *const pos, void *texture, float intensity, unsigned int blur, float scale, float addPos)
{
  (void)pos;
  (void)texture;
  (void)intensity;
  (void)blur;
  (void)scale;
  (void)addPos;
  // this is not a stub
}

void DrawOneMatGroup(CVertexBuffer *vertexBuffer, CIndexBuffer *indexBuffer, unsigned int vbMin, unsigned int vCnt, unsigned int ibMin, unsigned int triNum)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }

  if (vertexBuffer)
  {
    SetVPIPTex2(vertexBuffer->m_lpVertexBuffer, indexBuffer ? indexBuffer->m_lpIndexBuffer : nullptr);
  }
  device->DrawIndexedPrimitive(device, D3DPT_TRIANGLELIST, vbMin, vCnt, ibMin, triNum);
}

void DrawOneMatGroup(CVertexBuffer *vertexBuffer, _BSP_MAT_GROUP *matGroup)
{
  if (!matGroup)
  {
    return;
  }
  DrawOneMatGroup(vertexBuffer, nullptr, matGroup->VBMinIndex, matGroup->VCnt, matGroup->IBMinIndex, matGroup->TriNum);
}

void DrawReflectionOneMatGroup(CVertexBuffer *vertexBuffer, _BSP_MAT_GROUP *matGroup)
{
  DrawOneMatGroup(vertexBuffer, matGroup);
}

void DrawStripOneMatGroup(_BSP_MAT_GROUP *matGroup)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device || !matGroup)
  {
    return;
  }
  device->DrawIndexedPrimitive(device, D3DPT_TRIANGLELIST, matGroup->VBMinIndex, matGroup->VCnt, matGroup->IBMinIndex, matGroup->TriNum);
}

void SetMultiLayerCamera(float *const camera)
{
  if (!camera)
  {
    return;
  }
  g_multiLayerCamera[0] = camera[0];
  g_multiLayerCamera[1] = camera[1];
  g_multiLayerCamera[2] = camera[2];
}

float CN_GetDayTime()
{
  const unsigned int tick = GetLoopTime();
  return static_cast<float>(tick % 86400000u) / 86400000.0f;
}

__int64 CN_MixDayColor()
{
  if (dword_184A7981C)
  {
    return dword_184A7981C;
  }
  return 0xFFFFFFFF;
}

__int64 IsBBoxInFrustum(float *const bbMin, float *const bbMax)
{
  (void)bbMin;
  (void)bbMax;
  return 1;
}

__int64 IsBBoxInFrustum(short *const bbMin, short *const bbMax)
{
  (void)bbMin;
  (void)bbMax;
  return 1;
}

void DrawCParticle(CParticle *particle, CEntity *entity, float (*const world)[4], unsigned int color)
{
  if (!particle || !entity || !world)
  {
    return;
  }
  entity->DrawEntity(world, color, static_cast<double>(particle->mTotalTime));
}

void DrawAlphaStack(CBsp *bsp, float *const camera)
{
  (void)bsp;
  (void)camera;
  // this is not a stub
}

__int64 GetReflectionState()
{
  return g_reflectionState;
}

unsigned int GetDynamicLightNum()
{
  return g_dynamicLightCount;
}

void GetDynamicLight(unsigned int index, float *const pos, float *diameter, unsigned int *color, void **texture, unsigned int *blendType)
{
  if (index >= g_dynamicLightCount || !g_dynamicLights[index].used)
  {
    return;
  }

  const DynamicLightEntry &entry = g_dynamicLights[index];
  if (pos)
  {
    pos[0] = entry.pos[0];
    pos[1] = entry.pos[1];
    pos[2] = entry.pos[2];
  }
  if (diameter)
  {
    *diameter = entry.diameter;
  }
  if (color)
  {
    *color = entry.color;
  }
  if (texture)
  {
    *texture = entry.texture;
  }
  if (blendType)
  {
    *blendType = entry.blendType;
  }
}

__int64 IsMagicLight(unsigned int index)
{
  if (index >= g_dynamicLightCount || !g_dynamicLights[index].used)
  {
    return 0;
  }
  return g_dynamicLights[index].magic ? 1 : 0;
}

void UnSetDynamicLight()
{
  for (unsigned int i = 0; i < g_dynamicLightCount; ++i)
  {
    g_dynamicLights[i].used = false;
  }
  g_dynamicLightCount = 0;
}

void UnSetProjectedShadow()
{
  // this is not a stub
}

void SetDynamicLight(float *const pos, float diameter, unsigned int color, IDirect3DBaseTexture8 *texture, int multiTexture, unsigned int blendType)
{
  if (g_dynamicLightCount >= 16u)
  {
    return;
  }

  DynamicLightEntry &entry = g_dynamicLights[g_dynamicLightCount++];
  entry.used = true;
  entry.pos[0] = pos ? pos[0] : 0.0f;
  entry.pos[1] = pos ? pos[1] : 0.0f;
  entry.pos[2] = pos ? pos[2] : 0.0f;
  entry.diameter = diameter;
  entry.color = color;
  entry.texture = texture;
  entry.blendType = blendType;
  entry.magic = (multiTexture == 0);
}

void MultiTexOn()
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }
  device->SetTextureStageState(device, 1, D3DTSS_COLOROP, D3DTOP_MODULATE);
  device->SetTextureStageState(device, 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
}

void MultiTexOff()
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }
  device->SetTexture(device, 1, nullptr);
  device->SetTexture(device, 2, nullptr);
  device->SetTextureStageState(device, 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
  device->SetTextureStageState(device, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}

void BlendOn(int alphaType)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }

  device->SetRenderState(device, D3DRS_ALPHABLENDENABLE, 1);
  switch (alphaType)
  {
    case 1:
      device->SetRenderState(device, D3DRS_SRCBLEND, D3DBLEND_ONE);
      device->SetRenderState(device, D3DRS_DESTBLEND, D3DBLEND_ONE);
      break;
    default:
      device->SetRenderState(device, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
      device->SetRenderState(device, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
      break;
  }
}

void BlendOff()
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }
  device->SetRenderState(device, D3DRS_ALPHABLENDENABLE, 0);
}

__int64 IsEnableShader(unsigned int index)
{
  (void)index;
  return stState.mIsUsedVertexShader ? 1 : 0;
}

__int64 GetTextureMatrix(_R3MATERIAL *material, int layer, D3DXMATRIX *outMatrix, float time)
{
  (void)material;
  (void)layer;
  (void)time;
  if (!outMatrix)
  {
    return 0;
  }
  D3DXMatrixIdentity_0(outMatrix);
  return 0;
}

int GetTileAniTextureAddId(_R3MATERIAL *material, int layer, float time)
{
  (void)material;
  (void)layer;
  (void)time;
  return 0;
}

void MakeUV(_D3DR3VERTEX_TEX1 *vertex, int count, unsigned int *sourceUV, _R3MATERIAL *material, int layer)
{
  (void)material;
  (void)layer;
  if (!vertex || !sourceUV || count <= 0)
  {
    return;
  }

  for (int i = 0; i < count; ++i)
  {
    const unsigned int packed = sourceUV[i];
    const unsigned int u = packed & 0xFFFFu;
    const unsigned int v = (packed >> 16) & 0xFFFFu;
    vertex[i].u = static_cast<float>(u) / 65535.0f;
    vertex[i].v = static_cast<float>(v) / 65535.0f;
  }
}

void GetMatLightFromColor(D3DLIGHT8 *light, D3DMATERIAL8 *material, unsigned int color, float factor)
{
  const float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
  const float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
  const float b = static_cast<float>(color & 0xFF) / 255.0f;
  const float scaled = (factor > 0.0f) ? factor : 0.0f;

  if (light)
  {
    light->Diffuse.r = r * scaled;
    light->Diffuse.g = g * scaled;
    light->Diffuse.b = b * scaled;
    light->Diffuse.a = 1.0f;
    light->Ambient = light->Diffuse;
    light->Specular = light->Diffuse;
  }

  if (material)
  {
    material->Diffuse.r = r * scaled;
    material->Diffuse.g = g * scaled;
    material->Diffuse.b = b * scaled;
    material->Diffuse.a = 1.0f;
    material->Ambient = material->Diffuse;
    material->Specular = material->Diffuse;
    material->Emissive.r = 0.0f;
    material->Emissive.g = 0.0f;
    material->Emissive.b = 0.0f;
    material->Emissive.a = 1.0f;
    material->Power = 1.0f;
  }
}

void SetGrassVS(float addFrame, float *const factor, float alpha, int dayColor)
{
  (void)addFrame;
  (void)factor;
  (void)alpha;
  (void)dayColor;
  // this is not a stub
}

void SetLitGrassVS(float addFrame, float *const factor, float alpha, unsigned int dayColor, float *const transformedDir)
{
  (void)addFrame;
  (void)factor;
  (void)alpha;
  (void)dayColor;
  (void)transformedDir;
  // this is not a stub
}

void SetWorldViewMatrixVS(float (*const matrix)[4])
{
  (void)matrix;
  // this is not a stub
}

void R3DrawLine(float *const p0, float *const p1, unsigned int color)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device || !p0 || !p1)
  {
    return;
  }

  LineVertex line[2]{};
  line[0].x = p0[0];
  line[0].y = p0[1];
  line[0].z = p0[2];
  line[0].color = color;
  line[1].x = p1[0];
  line[1].y = p1[1];
  line[1].z = p1[2];
  line[1].color = color;

  device->SetVertexShader(device, D3DFVF_XYZ | D3DFVF_DIFFUSE);
  device->DrawPrimitiveUP(device, D3DPT_LINELIST, 1, line, sizeof(LineVertex));
}

unsigned int CN_GetFieldColor()
{
  return static_cast<unsigned int>(CN_MixDayColor());
}

void RTMovieFrameMove()
{
  // this is not a stub
}

void RTMovieRender()
{
  // this is not a stub
}

void RTMovieRelease()
{
  // this is not a stub
}

void SetDefaultFlags()
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device)
  {
    return;
  }

  device->SetRenderState(device, D3DRS_ZENABLE, 1);
  device->SetRenderState(device, D3DRS_CULLMODE, D3DCULL_CCW);
  device->SetRenderState(device, D3DRS_LIGHTING, 0);
  device->SetRenderState(device, D3DRS_ALPHABLENDENABLE, 0);
}

void SetFrustumNormalPlane()
{
  float (*m)[4] = stState.mMatViewProj;

  stState.mFrustumPlane[0][0] = m[0][3] + m[0][0];
  stState.mFrustumPlane[0][1] = m[1][3] + m[1][0];
  stState.mFrustumPlane[0][2] = m[2][3] + m[2][0];
  stState.mFrustumPlane[0][3] = m[3][3] + m[3][0];

  stState.mFrustumPlane[1][0] = m[0][3] - m[0][0];
  stState.mFrustumPlane[1][1] = m[1][3] - m[1][0];
  stState.mFrustumPlane[1][2] = m[2][3] - m[2][0];
  stState.mFrustumPlane[1][3] = m[3][3] - m[3][0];

  stState.mFrustumPlane[2][0] = m[0][3] + m[0][1];
  stState.mFrustumPlane[2][1] = m[1][3] + m[1][1];
  stState.mFrustumPlane[2][2] = m[2][3] + m[2][1];
  stState.mFrustumPlane[2][3] = m[3][3] + m[3][1];

  stState.mFrustumPlane[3][0] = m[0][3] - m[0][1];
  stState.mFrustumPlane[3][1] = m[1][3] - m[1][1];
  stState.mFrustumPlane[3][2] = m[2][3] - m[2][1];
  stState.mFrustumPlane[3][3] = m[3][3] - m[3][1];

  stState.mFrustumPlane[4][0] = m[0][2];
  stState.mFrustumPlane[4][1] = m[1][2];
  stState.mFrustumPlane[4][2] = m[2][2];
  stState.mFrustumPlane[4][3] = m[3][2];

  stState.mFrustumPlane[5][0] = m[0][3] - m[0][2];
  stState.mFrustumPlane[5][1] = m[1][3] - m[1][2];
  stState.mFrustumPlane[5][2] = m[2][3] - m[2][2];
  stState.mFrustumPlane[5][3] = m[3][3] - m[3][2];

  for (int i = 0; i < 6; ++i)
  {
    NormalizePlane(stState.mFrustumPlane[i]);
  }
}

void SetMultiLayerTime(float a1)
{
  g_multiLayerTime = a1;
}

void CN_NatureFrameMove()
{
  (void)g_multiLayerTime;
}

void UpdateStreamMP3()
{
  // this is not a stub
}

void DrawPrimitiveUPLine(unsigned int a1, void *a2)
{
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device || !a2)
  {
    return;
  }
  device->SetVertexShader(device, D3DFVF_XYZ | D3DFVF_DIFFUSE);
  device->DrawPrimitiveUP(device, D3DPT_LINELIST, a1, a2, sizeof(LineVertex));
}

int DrawFadeSKy()
{
  return 0;
}

void FramebufferToJPG(char *a1)
{
  (void)a1;
  // this is not a stub
}

int IsRayAABB(float *const a1, float *const a2, float *const a3, float *const a4, float (*a5)[3])
{
  float tMin = 0.0f;
  float tMax = 1.0f;
  const float dir[3] = {a4[0] - a3[0], a4[1] - a3[1], a4[2] - a3[2]};

  for (int axis = 0; axis < 3; ++axis)
  {
    if (std::fabs(dir[axis]) < 1e-6f)
    {
      if (a3[axis] < a1[axis] || a3[axis] > a2[axis])
      {
        return 0;
      }
      continue;
    }

    const float invDir = 1.0f / dir[axis];
    float t1 = (a1[axis] - a3[axis]) * invDir;
    float t2 = (a2[axis] - a3[axis]) * invDir;
    if (t1 > t2)
    {
      std::swap(t1, t2);
    }

    tMin = (tMin > t1) ? tMin : t1;
    tMax = (tMax < t2) ? tMax : t2;
    if (tMin > tMax)
    {
      return 0;
    }
  }

  if (a5)
  {
    (*a5)[0] = a3[0] + dir[0] * tMin;
    (*a5)[1] = a3[1] + dir[1] * tMin;
    (*a5)[2] = a3[2] + dir[2] * tMin;
  }
  return 1;
}

void SetReLoadState(int a1)
{
  dword_184A79820 = a1;
}

void ReLoadMaterial(char *a1, _R3MATERIAL *a2)
{
  (void)a1;
  (void)a2;
  // this is not a stub
}

__int64 D3DXMatrixPerspectiveFovLH_0(void *a1)
{
  if (!a1)
  {
    return 0;
  }

  const float fov = (stState.mFov > 0.0f) ? stState.mFov : 1.04719758f;
  const float aspect = (stState.mAsfectRatio > 0.0f) ? stState.mAsfectRatio : 1.33333337f;
  const float zn = (stState.mNear > 0.0f) ? stState.mNear : 1.0f;
  const float zf = (stState.mFar > zn) ? stState.mFar : 10000.0f;

  D3DXMATRIX matrix{};
  const float yScale = 1.0f / std::tan(fov * 0.5f);
  const float xScale = yScale / aspect;
  matrix._11 = xScale;
  matrix._22 = yScale;
  matrix._33 = zf / (zf - zn);
  matrix._34 = 1.0f;
  matrix._43 = (-zn * zf) / (zf - zn);
  matrix._44 = 0.0f;

  std::memcpy(a1, &matrix, sizeof(matrix));
  std::memcpy(stState.mMatProj, &matrix, sizeof(matrix));
  D3DXMatrixMultiply_0(
    reinterpret_cast<D3DXMATRIX *>(&stState.mMatViewProj[0][0]),
    reinterpret_cast<const D3DXMATRIX *>(&stState.mMatView[0][0]),
    reinterpret_cast<const D3DXMATRIX *>(&stState.mMatProj[0][0]));
  return reinterpret_cast<__int64>(a1);
}

void GetFrustumNormalPlane(float (*const a1)[4])
{
  if (!a1)
  {
    return;
  }
  std::memcpy(a1, stState.mFrustumPlane, sizeof(stState.mFrustumPlane));
}

void R3GetQuakeVector(float *const a1)
{
  if (!a1)
  {
    return;
  }
  a1[0] = 0.0f;
  a1[1] = 0.0f;
  a1[2] = 0.0f;
}

void DrawTestBox(float a1, float a2, float a3, unsigned int a4)
{
  float p0[3]{a1 - 0.5f, a2, a3};
  float p1[3]{a1 + 0.5f, a2, a3};
  float p2[3]{a1, a2 - 0.5f, a3};
  float p3[3]{a1, a2 + 0.5f, a3};
  float p4[3]{a1, a2, a3 - 0.5f};
  float p5[3]{a1, a2, a3 + 0.5f};
  R3DrawLine(p0, p1, a4 | 0xFF000000u);
  R3DrawLine(p2, p3, a4 | 0xFF000000u);
  R3DrawLine(p4, p5, a4 | 0xFF000000u);
}
