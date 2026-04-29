#pragma once

#include "../Character.h"

#include <map>
#include <string>
#include <vector>

class CParser
{
public:
  CParser(void);
  virtual ~CParser(void);

  std::string GetName(void);
  virtual void Parsing(std::string &pi_rToken);

  std::string m_sName;
  int m_nStatus;
};

class CScript
{
public:
  CScript(void);
  ~CScript(void);

  int AddParser(CParser *pi_pParser);
  static int SetParser(CParser *pi_pParser);
  static int SetParser(std::string &pio_rName);
  static CParser *GetParser(char *pi_pName);
  static CParser *GetParser(std::string &pio_rName);
  static int GetToken(std::string &po_rToken);
protected:
  char *StrTok(char *pi_pString,
               char *pi_pSkip,
               char *pi_pDelimiter,
               unsigned long pi_dwLength,
               unsigned long &pio_rOffset);
public:
  char *StrTok(char *pi_pString,
               char *pi_pSkip,
               char *pi_pDelimiter,
               unsigned int pi_dwLength,
               unsigned int *pio_pdwOffset);
  void ScriptLexer(char *pi_pString);
  void ScriptFlieLexer(char *pi_pFileName);
  int ScriptParser(void);

  static CScript *m_pScript;

private:
  int m_nState;
  std::vector<std::string> m_vecTokens;
  size_t m_stTokenIndex;
  std::map<std::string, CParser *> m_mapParsers;

  static CScript *s_pCurrentScript;
  static CParser *s_pCurrentParser;
};

class EffectObject
{
public:
  BYTE m_abData[292];

  EffectObject(void);
  EffectObject(const EffectObject &pi_rOther);
  ~EffectObject(void);
};

class CEffectMap
{
public:
  std::map<std::string, EffectObject> m_mapObjects;

  CEffectMap(void);
  CEffectMap(const CEffectMap &pi_rOther);
  ~CEffectMap(void);
};

class EffectFileList
{
public:
  EffectFileList(void);

  void LoadEffectFileList(char *pi_pFileName);
};

class EffectList
{
public:
  void LoadEffectFileList(char *pi_pFileName);
};

class EffectPatternList
{
public:
  void LoadEffectPatternList(char *pi_pFileName);
};

class ItemEffectList
{
public:
  ItemEffectList(void);
  ~ItemEffectList(void);

  void LoadItemListFile(char *pi_pFileName);
};

class EffectObjectManager
{
public:
  EffectObjectManager(void);
  ~EffectObjectManager(void);

  static EffectObjectManager m_This;

  static void Clear(void);
  static void LoadEffect(char *pi_pFileName);
  static void ExportEffect(char *pi_pFileName, char *pi_pOutFileName);
  static int GetEffect(char pi_chType,
                       int pi_nEffectID,
                       int pi_nPass,
                       int pi_nLevel,
                       DWORD *po_pdwEffectID,
                       int pi_nFallback);
  static int AddEffect(char pi_chType, int pi_nEffectID, char *pi_pFileName);
  static int AddEffect(char pi_chType,
                       int pi_nEffectID,
                       int pi_nArg3,
                       int pi_nArg4,
                       char pi_chArg5);
  static void AddEffect(EffectObject pi_stEffectObject);
  static int SetEffect(void);
  static int SetEffect(CHARACTEROBJECT *pi_pCharacterObject, char *pi_pEffectName);
  static void DelEffect(int pi_nEffectID, int pi_nLevel);
  static int SetPartEffect(CHARACTEROBJECT *pi_pCharacterObject,
                           char *pi_pPartName,
                           char *pi_pEffectName);
  static int SetPartEffect(CHARACTEROBJECT *pi_pCharacterObject,
                           char *pi_pPartName,
                           int pi_nLevel);
  void SetEffectToMesh(ObjectMesh *pio_pObjectMesh,
                       EffectObject *pi_pEffectObject,
                       int pi_nPassLevel);
  void SetEffectToMesh(CHARACTEROBJECT *pi_pCharacterObject,
                       DWORD pi_dwMeshID,
                       DWORD pi_dwLevel);
  void SetPartEffectToMesh(ObjectMesh *pio_pObjectMesh,
                           char pi_chType,
                           int pi_nArg4,
                           int pi_nArg5,
                           int pi_nArg6,
                           int pi_nArg7);
  void SetPartEffectToMesh(CHARACTEROBJECT *pi_pCharacterObject,
                           char *pi_pPartName,
                           char *pi_pEffectName);
};

class EffectMainParser
{
public:
  static void Parsing(int pi_nArg);
};
bool IsHaveMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                      DWORD pi_dwMeshID,
                      DWORD pi_dwLevel,
                      DWORD pi_dwArg4);
void SetMeshEffect(CHARACTEROBJECT *pi_pCharacterObject,
                   DWORD pi_dwMeshID,
                   DWORD pi_dwLevel,
                   DWORD pi_dwArg4);

ObjectData *ImportMeshData(char *pi_pFileName);
ObjectData *MakeMeshData(char *pi_pBuffer);
ObjectMesh *AddObjectMesh(ObjectMesh *pi_pBaseMesh,
                          unsigned long pi_dwBaseCount,
                          ObjectMesh *pi_pAppendMesh,
                          unsigned long pi_dwAppendCount);
ObjectMesh *FindParent(char *pi_pName, ObjectMesh *pi_pObjectMesh, int pi_nCount);
int FindParent(ObjectMesh *pi_pParentMesh,
               int pi_nParentCount,
               ObjectMesh *pio_pChildMesh,
               int pi_nChildCount);
void FindBlendParent(ObjectMesh *pi_pParentMesh,
                     int pi_nParentCount,
                     ObjectMesh *pio_pChildMesh,
                     int pi_nChildCount,
                     ObjectData *pi_pObjectData);
int RematchParent(ObjectMesh *pi_pParentMesh,
                  int pi_nParentCount,
                  ObjectMesh *pio_pChildMesh,
                  int pi_nChildCount);
void ConvertVtToD3D(CHARACTERVERTEX *po_pVertex, EXPTVERTEX *pi_pVertex);
void ConvertVtToD3D(CHARACTERVERTEX &po_rVertex, EXPTVERTEX &pi_rVertex);
int ConvertUV(CHARACTERVERTEX *pio_pVertex, float pi_fU, float pi_fV);
void ConvertUV(CHARACTERVERTEX &pio_rVertex, tagTEXCOORDFLOAT pi_stTexCoord);
void DecodeTwoBlendPhysiqueData(CHARACTERVERTEX *pio_pVertex,
                                int pi_nVertexCount,
                                PHYSIQUE *pi_pPhysique,
                                int pi_nPhysiqueCount);
void ImportAnimationData(CHARACTEROBJECT *pio_pCharacterObject, unsigned char *pi_pAnimationBytes);
int MatchAnimationData(const unsigned char *pi_pAnimationHeader,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       unsigned char *pi_pAnimationBytes);
int MatchAnimationData(AnimationHeader *pi_pAnimationHeader,
                       AnimationData *pi_pAnimationData,
                       CHARACTEROBJECT *pio_pCharacterObject,
                       int pi_nStartIndex);
void ImportTextureData(TextureManager *pi_pTextureManager, char *pi_pPathName, char *pi_pFileName);
void FindMaxAni(ChAnimation *pio_pAnimation, unsigned char *pi_pAnimationBytes);
unsigned char *ReadAnimationData(char *pi_pFileName);
void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager);
void InitVertex(ObjectData *pi_pObjectData,
                CHARACTEROBJECT *pi_pParentBone,
                CHARACTEROBJECT *pio_pCharacterObject,
                TextureManager *pi_pTextureManager,
                bool pi_bUseImmediateFile);
void ScaleLoad(char *pi_pFileName, CHARACTEROBJECT *pio_pCharacterObject);
int ImportBoundBox(char *pi_pFileName, char *po_pMin, char *po_pMax);
int ImportBoundBox(char *pi_pFileName,
                   char *const pi_pPathName,
                   float *const po_pfMin,
                   float *const po_pfMax);
unsigned int LoadAndCreateShader(int pi_nArg1,
                                 DWORD pi_dwBytes,
                                 int pi_nArg3,
                                 int pi_nArg4,
                                 int pi_nArg5);

FILE *CHGetFileSize(char *pi_pFileName);
int CHGetMipMapSkipSize(DWORD *pi_pHeader,
                        unsigned int pi_dwWidth,
                        unsigned int pi_dwHeight,
                        unsigned int pi_dwMipLevel);
IDirect3DTexture8 *CreateTexture(IDirect3DDevice8 *pi_pDevice,
                                 unsigned long pi_dwWidth,
                                 unsigned long pi_dwHeight);
IDirect3DSurface8 *CreateSurface(IDirect3DDevice8 *pi_pDevice,
                                 unsigned long pi_dwWidth,
                                 unsigned long pi_dwHeight);
IDirect3DTexture8 *CHR3LoadDDS(IDirect3DDevice8 *pi_pDevice,
                               char *pi_pFileName,
                               int pi_nArg3,
                               int pi_nArg4,
                               int pi_nArg5,
                               int pi_nArg6);
IDirect3DTexture8 *CHR3LoadDDS(IDirect3DDevice8 *pi_pDevice,
                               char *pi_pFileName,
                               unsigned long pi_dwSize,
                               bool pi_bMipMap,
                               unsigned long pi_dwArg5,
                               unsigned long pi_dwArg6,
                               unsigned long pi_dwArg7);
int CHR3LoadDDSFromFP(IDirect3DDevice8 *pi_pDevice,
                      FILE *pi_pFile,
                      size_t pi_stSize,
                      IDirect3DTexture8 **po_ppTexture,
                      int pi_nArg5,
                      int pi_nArg6,
                      int pi_nArg7);
DWORD *FixDDSHeader(DWORD *pio_pHeader);
void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  IDirect3DTexture8 *pi_pTexture,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  DWORD pi_dwColor);
void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  IDirect3DTexture8 *pi_pTexture,
                  RECT *pi_pSrcRect,
                  float pi_fX,
                  float pi_fY,
                  DWORD pi_dwColor);
void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  void *pi_pTexture,
                  unsigned long pi_dwColor);
void Draw2DSprite(IDirect3DDevice8 *pi_pDevice,
                  float pi_fX,
                  float pi_fY,
                  float pi_fWidth,
                  float pi_fHeight,
                  float *const pi_pfUV0,
                  float *const pi_pfUV1,
                  void *pi_pTexture,
                  unsigned long pi_dwColor);

void _R3CalculateTime(void);
void InitMaterial(D3DMATERIAL8 *po_pMaterial,
                  float pi_fRed,
                  float pi_fGreen,
                  float pi_fBlue,
                  float pi_fAlpha);
void InitMaterial(D3DMATERIAL8 &po_rMaterial,
                  float pi_fRed,
                  float pi_fGreen,
                  float pi_fBlue,
                  float pi_fAlpha);
void GetNormal(D3DXVECTOR3 *po_pNormal,
               D3DXVECTOR3 *pi_p0,
               D3DXVECTOR3 *pi_p1,
               D3DXVECTOR3 *pi_p2);
bool FindEdgeAddNormal(EDGE *pio_pEdges,
                       int pi_nEdgeCount,
                       EDGE *pi_pEdge,
                       D3DXVECTOR3 *pi_pNormal);
bool FindEdgeAddNormal(EDGE *pio_pEdges,
                       int pi_nEdgeCount,
                       EDGE *pi_pEdge,
                       D3DXVECTOR3 &pio_rNormal);
void AddEdge(EDGE *pio_pEdges, EDGE *pi_pEdge, int *pio_pEdgeCount, D3DXVECTOR3 *pi_pNormal);
void AddEdge(EDGE *pio_pEdges, EDGE *pi_pEdge, int &pio_rEdgeCount, D3DXVECTOR3 &pio_rNormal);
void MeshAlphaBlend(unsigned int pi_dwFrame, ObjectMesh *pio_pObjectMesh);
void MeshAlphaBlend(int pi_nFrame, ObjectMesh *pio_pObjectMesh);
void MeshRotBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
void MeshRotBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
void MeshScaleBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
void MeshScaleBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
void MeshTransBlend(D3DXMATRIX *pio_pMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
void MeshTransBlend(D3DXMATRIX &pio_rMatrix, int pi_nFrame, ObjectMesh *pi_pObjectMesh);
int SetTextureEffect(int pi_nArg1, int pi_nArg2, float pi_fArg3);
enum EFFECTWAVETYPE
{
  EFFECTWAVETYPE_NONE = 0
};
void SetTextureEffect(unsigned long pi_dwStage,
                      EFFECTWAVETYPE pi_eWaveType,
                      float pi_fFrame,
                      float pi_fSpeed);
void DrawP(IDirect3DDevice8 *pi_pDevice, int pi_nArg2);
void DrawP(IDirect3DDevice8 *pi_pDevice, int pi_nArg2, int pi_nArg3);

void SetFustumNormalPlane(float (*const pio_pPlane)[4], D3DXMATRIX &pio_rMatrix);

extern EffectFileList g_EffectFileList;
extern ItemEffectList g_EffectItemList;
extern EffectList g_EFFMonList;
extern EffectList g_EFFNPCList;
extern EffectList g_EFFUnitList;
extern EffectPatternList g_PatternList;
