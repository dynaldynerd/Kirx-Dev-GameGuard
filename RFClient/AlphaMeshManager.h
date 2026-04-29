#pragma once

struct AlphaMesh
{
  float m_fViewZ;
  AlphaMesh *m_pNext;
  unsigned long m_dwMatrixCount;
  float m_afMatrix[16];
  unsigned char m_abData[0x100 - 0x0C - (sizeof(float) * 16)];
};

class AlphaMeshManager
{
public:
  AlphaMesh m_aAlphaMesh[1024];
  int m_nAlphaMeshCount;
  float m_afViewMatrix[16];
  AlphaMesh *m_pHead;

  void AddAlphaMesh(AlphaMesh &pio_rAlphaMesh);
  void AddAlphaMesh(struct ObjectMesh *pi_pObjectMesh,
                    class CHARACTEROBJECT *pi_pCharacterObject,
                    struct D3DXMATRIX *pi_pWorldMatrix,
                    float pi_fScroll);
  float GetViewZ(AlphaMesh *pi_pAlphaMesh);
  float GetViewZ(struct D3DXMATRIX *pi_pWorldMatrix);
  void DrawAlpahMesh(void);
};

static_assert(sizeof(AlphaMesh) == 0x100, "AlphaMesh size mismatch");
static_assert(sizeof(AlphaMeshManager) == 0x40048, "AlphaMeshManager size mismatch");

extern AlphaMeshManager g_AMeshManager;
