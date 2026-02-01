#include "pch.h"
#include "CBsp.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CEntity.h"
#include "CParticle.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <intrin.h>

namespace
{
    using GetVertexFromCompressFn = void (*)(float *, char *, _BSP_READ_M_GROUP *);

    void GetVertexFromBVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromBVertex(dst, src, mg);
    }

    void GetVertexFromWVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromWVertex(dst, reinterpret_cast<short *>(src), mg);
    }

    void GetVertexFromFVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromFVertex(dst, reinterpret_cast<float *>(src), mg);
    }

    GetVertexFromCompressFn GetVertexFromCompress[5] = {
        nullptr,
        GetVertexFromBVertexWrap, // 1
        GetVertexFromWVertexWrap, // 2
        nullptr,
        GetVertexFromFVertexWrap  // 4
    };
}

float CBsp::GetFirstYpos(float *const a2, float *const a3, float *const a4)
{
    float v6 = -65535.0f;
    short v61[32000];
    int v43 = 0;
    
    // a4[1] is max, a3[1] is min
    float v46[3] = {a2[0], (float)a4[1], a2[2]};
    float v49[3] = {a2[0], (float)a3[1], a2[2]};

    this->GetLeafList(v46, v49, &v43, v61, 32000);
    if (v43 > 0)
    {
        for (int i = 0; i < v43; ++i)
        {
            _BSP_LEAF &leaf = this->mLeaf[v61[i]];
            for (unsigned int j = 0; j < leaf.face_num; ++j)
            {
                unsigned int faceIdx = this->mCFaceId[leaf.face_start_id + j];
                _BSP_C_FACE &face = this->mCFace[faceIdx];
                
                if (!(face.Attr & 0x40))
                {
                    float v54[3];
                    if (GetPlaneCrossPoint(v46, v49, v54, face.Normal, face.Normal[3]))
                    {
                        float v22 = v54[1];
                        if (v46[1] > v22 && v22 > v49[1] && v22 > v6)
                        {
                            v6 = v22;
                            this->mNowCFaceId = (int)faceIdx;
                        }
                    }
                }
            }
        }
    }
    return v6;
}

void CBsp::GetLeafList(float *const a2, float *const a3, int *const a4, __int16 *a5, unsigned int a6)
{
    if (!a4) return;
    int count = 0;
    this->WalkNodeForLeafList(1, a2, a3, a5, &count);
    *a4 = count;
}

void CBsp::WalkNodeForLeafList(short nNodeId, float *const a3, float *const a4, short *a5, int *a6)
{
    if (nNodeId == 0) return;
    _BSP_NODE &node = this->mNode[nNodeId];
    float *normal = this->mCNNormal[node.f_normal_id];
    
    float v11 = DotProduct(normal, a3) - node.d;
    float v12 = DotProduct(normal, a4) - node.d;
    
    if ((v11 > 0.0f || v12 <= 0.0f) && (v12 > 0.0f || v11 <= 0.0f))
    {
        this->SubLeafList(v11, &node, a3, a4, a5, a6);
    }
    else
    {
        float v13 = v11 / (v11 - v12);
        float intersection[3];
        intersection[0] = (a4[0] - a3[0]) * v13 + a3[0];
        intersection[1] = (a4[1] - a3[1]) * v13 + a3[1];
        intersection[2] = (a4[2] - a3[2]) * v13 + a3[2];
        
        this->SubLeafList(v11, &node, a3, intersection, a5, a6);
        this->SubLeafList(v12, &node, intersection, a4, a5, a6);
    }
}

void CBsp::SubLeafList(float dist, _BSP_NODE *pNode, float *const a4, float *const a5, short *a6, int *a7)
{
    short next = (dist >= 0.0f) ? pNode->front : pNode->back;
    if (next < 0)
    {
        a6[(*a7)++] = -1 - next;
    }
    else
    {
        this->WalkNodeForLeafList(next, a4, a5, a6, a7);
    }
}

void CBsp::LoadBsp(char *a2)
{
    if (!IsInitR3Engine())
        Error(aR3engineAe, byte_140883769);

    SetMergeFileManager(nullptr);

    FILE *v4 = fopen(a2, "rb");
    if (!v4)
        Error(a2, aAaiai_0);

    unsigned int v5 = 0;
    mNowCFaceId = 0;
    mTotalAllocSize = 0;
    ResetTotalVertexBufferInfo();

    fread(&mBSPHeader, 0x2ACu, 1, v4);
    if (mBSPHeader.version != 39)
        Error(aBspAai, aAa);

    unsigned __int128 v6 = (unsigned __int128)mBSPHeader.Leaf.size * 0x47AE147AE147AE15ull;
    mLeafNum = static_cast<unsigned int>(((static_cast<unsigned long long>(v6 >> 64) +
                                           ((mBSPHeader.Leaf.size - static_cast<unsigned long long>(v6 >> 64)) >> 1)) >>
                                          4));
    mNodeNum = mBSPHeader.Node.size / 0x18;
    mCVertexNum = mBSPHeader.BVertex.size / 3 + mBSPHeader.WVertex.size / 6 + mBSPHeader.FVertex.size / 0xC;
    mCFaceNum = mBSPHeader.Face.size / 6;
    mObjectNum = mBSPHeader.Object.size / 0x58;

    unsigned __int128 v9 = (unsigned __int128)mBSPHeader.ReadMatGroup.size * 0x8618618618618619ull;
    mMatGroupNum = static_cast<unsigned int>(((static_cast<unsigned long long>(v9 >> 64) +
                                                ((mBSPHeader.ReadMatGroup.size - static_cast<unsigned long long>(v9 >> 64)) >> 1)) >>
                                               5));

    int v10 = 12 * mCVertexNum;
    unsigned int size = mBSPHeader.VertexId.size;
    int v12 = 24 * mCFaceNum;

    if (IsServerMode())
    {
        mCNEdgeNormal = (float (*)[4])Dmalloc(mCFaceNum << 6);
        mMapEntitiesListNum = 0;
        mEntityIDNum = 0;
        mEntityListNum = 0;
        mLeafEntityListNum = 0;
    }

    unsigned int v13 = v10 + size;
    unsigned __int128 v14 = (unsigned __int128)mBSPHeader.ReadMatGroup.size * 0x8618618618618619ull;
    unsigned int matGroupNum =
        static_cast<unsigned int>(((static_cast<unsigned long long>(v14 >> 64) +
                                 ((mBSPHeader.ReadMatGroup.size - static_cast<unsigned long long>(v14 >> 64)) >> 1)) >>
                                5));

    unsigned int v15 =
        v12 + v13 + mBSPHeader.CPlanes.size + mBSPHeader.CFaceId.size + mBSPHeader.Node.size + mBSPHeader.Track.size +
        mBSPHeader.Leaf.size + mBSPHeader.LgtUV.size + mBSPHeader.MatListInLeaf.size + mBSPHeader.VertexColor.size +
        361 * (mBSPHeader.Object.size / 0x58) +
        2 * (mObjectNum + 29 * mMapEntitiesListNum + 43 * matGroupNum);

    mStaticAllocSize = v15;
    mTotalAllocSize += v15;
    mStaticAlloc = static_cast<unsigned char *>(Dmalloc(v15));

    mCVertex = reinterpret_cast<float (*)[3]>(mStaticAlloc);
    mCVertexId = reinterpret_cast<unsigned int *>(&mStaticAlloc[v10]);
    mCFace = reinterpret_cast<_BSP_C_FACE *>(&mStaticAlloc[v13]);

    unsigned int v17 = v12 + v13;
    mCNNormal = reinterpret_cast<float (*)[3]>(&mStaticAlloc[v17]);
    fread(mCNNormal, mBSPHeader.CPlanes.size, 1, v4);

    unsigned int v19 = mBSPHeader.CPlanes.size + v17;
    mCFaceId = reinterpret_cast<unsigned int *>(&mStaticAlloc[v19]);
    fread(mCFaceId, mBSPHeader.CFaceId.size, 1, v4);

    unsigned int v21 = mBSPHeader.CFaceId.size + v19;
    mNode = reinterpret_cast<_BSP_NODE *>(&mStaticAlloc[v21]);
    fread(mNode, mBSPHeader.Node.size, 1, v4);

    unsigned int v23 = mBSPHeader.Node.size + v21;
    mLeaf = reinterpret_cast<_BSP_LEAF *>(&mStaticAlloc[v23]);
    fread(mLeaf, mBSPHeader.Leaf.size, 1, v4);

    unsigned int v25 = mBSPHeader.Leaf.size + v23;
    MatListInLeafId = reinterpret_cast<unsigned short *>(&mStaticAlloc[v25]);
    fread(MatListInLeafId, mBSPHeader.MatListInLeaf.size, 1, v4);

    unsigned int v27 = mBSPHeader.MatListInLeaf.size + v25;
    unsigned int *v28 = static_cast<unsigned int *>(Dmalloc(mBSPHeader.Object.size));
    mObject = reinterpret_cast<_ANI_OBJECT *>(&mStaticAlloc[v27]);
    fread(v28, mBSPHeader.Object.size, 1, v4);

    unsigned int v29 = 361 * (mBSPHeader.Object.size / 0x58) + v27;
    unsigned char *v30 = &mStaticAlloc[v29];
    fread(v30, mBSPHeader.Track.size, 1, v4);

    unsigned int v31 = mBSPHeader.Track.size + v29;
    mEventObjectID = reinterpret_cast<unsigned short *>(&mStaticAlloc[v31]);
    fread(mEventObjectID, mBSPHeader.EventObjectID.size, 1, v4);

    unsigned int v33 = v31 + 2 * mObjectNum;
    int v34 = 0;
    unsigned int *p_size = &mBSPHeader.ReadSpare[0].size;
    __int64 v36 = 35;
    do
    {
        v34 += *p_size;
        p_size += 2;
        --v36;
    } while (v36);
    fseek(v4, v34, 1);

    ConvAniObject(static_cast<int>(mBSPHeader.Object.size / 0x58), v30, reinterpret_cast<_READ_ANI_OBJECT *>(v28), mObject);
    Dfree(v28);

    mMatGroup = reinterpret_cast<_BSP_MAT_GROUP *>(&mStaticAlloc[v33]);
    unsigned __int128 v38 = (unsigned __int128)mBSPHeader.ReadMatGroup.size * 0x8618618618618619ull;
    unsigned int v39 =
        86 * static_cast<unsigned int>(((static_cast<unsigned long long>(v38 >> 64) +
                                         ((mBSPHeader.ReadMatGroup.size - static_cast<unsigned long long>(v38 >> 64)) >> 1)) >>
                                        5)) +
        v33;
    mLgtUV = reinterpret_cast<__int16 (*)[2]>(&mStaticAlloc[v39]);
    mVertexColor = reinterpret_cast<unsigned int *>(&mStaticAlloc[mBSPHeader.LgtUV.size + v39]);

    ReadDynamicDataFillVertexBuffer(v4);
    fclose(v4);

    unsigned __int128 v40 = (unsigned __int128)mBSPHeader.ReadMatGroup.size * 0x8618618618618619ull;
    mMatGroupCacheSize = static_cast<int>(((static_cast<unsigned long long>(v40 >> 64) +
                                            ((mBSPHeader.ReadMatGroup.size - static_cast<unsigned long long>(v40 >> 64)) >>
                                             1)) >>
                                           8) +
                                          1);
    mMatGroupCache = static_cast<unsigned char *>(Dmalloc(mMatGroupCacheSize));
    mTotalAllocSize += mMatGroupCacheSize;

    mAlpha.InitAlpha(this);

    mMapEntityMFM.InitMergeFile(byte_184A790F0);
    SetMergeFileManager(&mMapEntityMFM);

    unsigned int v42 = 0;
    int *mEnvIDPtr = reinterpret_cast<int *>(mEnvID);
    do
    {
        int v46 = unk_184A7999C[v42];
        mEnvIDPtr[0] = v46;
        if (v46)
        {
            char *v47 = &byte_184A79924[128 * v42];
            if (IsParticle(v47))
                mEnvIDPtr[0] |= 0x1000u;
            if (_bittest(mEnvIDPtr, 0xCu))
            {
                CParticle *v48 = reinterpret_cast<CParticle *>(operator new(0x490ull));
                CParticle *v49 = v48 ? new (v48) CParticle() : nullptr;
                ___u21.mEnvEntity[v42] = reinterpret_cast<CEntity *>(v49);
                if (v49 && CParticle::LoadParticleSPT(v49, v47, 0))
                {
                    CParticle::InitParticle(v49);
                    CParticle::SetParticleState(v49, 1u);
                }
                else
                {
                    CParticle *v50 = reinterpret_cast<CParticle *>(___u21.mEnvEntity[v42]);
                    if (___u21.mEnvEntity[v42])
                    {
                        v50->~CParticle();
                        operator delete(v50);
                    }
                    mEnvIDPtr[0] = 0;
                    unk_184A7999C[v42] = 0;
                }
            }
            else
            {
                CEntity *v51 = reinterpret_cast<CEntity *>(operator new(0xF4ull));
                CEntity *v52 = v51 ? new (v51) CEntity() : nullptr;
                ___u21.mEnvEntity[v42] = v52;
                if (v52 && !CEntity::LoadEntity(v52, v47, 0))
                {
                    CEntity *v53 = ___u21.mEnvEntity[v42];
                    if (v53)
                    {
                        v53->~CEntity();
                        operator delete(v53);
                    }
                }
            }
        }
        ++v42;
        ++mEnvIDPtr;
    } while (v42 < 2);

    if (mObjectNum)
    {
        unsigned int v54 = 0;
        do
        {
            if (mObject[v54].parent >= mObjectNum)
                mObject[v54].parent = 0;
            ++v5;
            ++v54;
        } while (v5 < mObjectNum);
    }
}

bool CBsp::LoadExtBsp(char *szFileName)
{
    this->mIsLoadEBP = 1;
    return true;
}

bool CBsp::EdgeTest(float *pPos, unsigned int nLineId)
{
    _TOOL_COL_LINE &line = this->mCFLine[nLineId];
    float *v = (float*)this->mCFVertex;
    float *n = this->mCFNormal[nLineId];

    float p1[3] = { v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2] };
    float p2[3] = { v[3 * line.start_v], v[3 * line.start_v + 1] + line.height, v[3 * line.start_v + 2] };

    if (!CheckEdgeEpsilon(p1, p2, pPos, n)) return false;

    float p3[3] = { v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p2, p3, pPos, n)) return false;

    float p4[3] = { v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p3, p4, pPos, n)) return false;

    return CheckEdgeEpsilon(p4, p1, pPos, n);
}

bool CBsp::CanYouGoThere(float *const a2, float *const a3, float (*a4)[3]) { 
    return !this->mIsLoaded || this->GetPathCrossPoint(a2, a3, a4, 0, 0) == 0;
}

int CBsp::GetPathCrossPoint(float *const a2, float *const a3, float (*a4)[3], int a5, int a6)
{
    this->mColFaceId = -1;
    if (this->mCFLineNum < 2) return 0;

    float p1[3] = { a2[0], a2[1] + 16.0f, a2[2] };
    float p2[3] = { a3[0], a3[1] + 16.0f, a3[2] };

    short v66[32000];
    int v53 = 0;
    this->GetLeafList(p1, p2, &v53, v66, 32000);

    float minDist = 100000.0f;
    int hit = 0;

    for (int i = 0; i < v53; ++i)
    {
        _TOOL_COL_LEAF &leaf = this->mCFLeaf[v66[i]];
        for (unsigned int j = 0; j < leaf.line_num; ++j)
        {
            unsigned int lineIdx = this->mCFLineId[leaf.start_id + j];
            if (lineIdx == a5 || lineIdx == a6) continue;

            float *n = this->mCFNormal[lineIdx];
            float d1 = DotProduct(n, p1) - n[3];
            float d2 = DotProduct(n, p2) - n[3];

            if ((d1 > 0.0f && d2 > 0.0f) || (d1 < -0.01f && d2 < -0.01f)) continue;

            float t = d1 / (d1 - d2);
            float intersect[3] = { (p2[0] - p1[0]) * t + p1[0], (p2[1] - p1[1]) * t + p1[1], (p2[2] - p1[2]) * t + p1[2] };

            if (this->EdgeTest(intersect, lineIdx))
            {
                float dist = sqrtf((intersect[0] - p1[0]) * (intersect[0] - p1[0]) + (intersect[1] - p1[1]) * (intersect[1] - p1[1]) + (intersect[2] - p1[2]) * (intersect[2] - p1[2]));
                if (dist < minDist)
                {
                    minDist = dist;
                    this->mColFaceId = lineIdx;
                    if (a4) memcpy(a4, intersect, 12);
                    hit = 1;
                }
            }
        }
    }

    if (hit) return 1;
    return 0;
}

void CBsp::CalcEntitiesMainColor() {}

void CBsp::SetCFNormal()
{
    if (this->mCFLineNum)
    {
        for (unsigned int i = 0; i < this->mCFLineNum; ++i)
        {
            _TOOL_COL_LINE &line = this->mCFLine[i];
            float *v = (float*)this->mCFVertex;
            float v20[3] = { v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2] };
            float v19[3] = { v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2] };
            float v18[3] = { v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2] };
            GetNormal(this->mCFNormal[i], v20, v19, v18);
        }
    }
}

void CBsp::OnlyStoreCollisionStructure(_BSP_READ_M_GROUP *pRM, char (*pBV)[3], short (*pWV)[3], float (*pFV)[3], unsigned int *pVI, _BSP_READ_FACE *pRF, unsigned int *pFI)
{
    unsigned int vTotal = this->mBSPHeader.FVertex.size / 12 + this->mBSPHeader.BVertex.size / 3 + this->mBSPHeader.WVertex.size / 6;
    this->mCVertex = (float (*)[3])new float[vTotal * 3];
    this->mCVertexId = new unsigned int[this->mBSPHeader.VertexId.size / 4];
    this->mCFace = new _BSP_C_FACE[this->mCFaceNum];

    for (int i = 0; i < (int)(this->mBSPHeader.ReadMatGroup.size / 0x2A); ++i)
    {
        _BSP_READ_M_GROUP &mGroup = pRM[i];
        unsigned char type = 4;
        if (mGroup.attr & 0x8000) type = 1; else if (mGroup.attr & 0x4000) type = 2;

        for (unsigned int j = 0; j < mGroup.face_num; ++j)
        {
            unsigned int faceIdx = pFI[j + mGroup.face_start_id];
            _BSP_READ_FACE &readFace = pRF[faceIdx];
            _BSP_C_FACE &face = this->mCFace[faceIdx];
            
            face.Attr = 0;
            face.VNum = readFace.v_num;
            face.VStartId = 0; 
            face.MatGIndex = i;

            for (unsigned int k = 0; k < readFace.v_num; ++k)
            {
                unsigned int vId = pVI[k + readFace.v_start_id];
                float pos[3];
                if (type == 1) GetVertexFromBVertex(pos, pBV[vId], &mGroup);
                else if (type == 2) GetVertexFromWVertex(pos, pWV[vId], &mGroup);
                else GetVertexFromFVertex(pos, pFV[vId], &mGroup);

                memcpy(this->mCVertex[vId], pos, 12);
                this->mCVertexId[k + readFace.v_start_id] = vId;
            }

            if (readFace.v_num >= 3)
            {
                unsigned int v0 = this->mCVertexId[readFace.v_start_id];
                unsigned int v1 = this->mCVertexId[readFace.v_start_id + 1];
                unsigned int v2 = this->mCVertexId[readFace.v_start_id + 2];
                GetNormal(face.Normal, this->mCVertex[v0], this->mCVertex[v1], this->mCVertex[v2]);
            }
        }
    }
}

void CBsp::ReadDynamicDataFillVertexBuffer(FILE *Stream)
{
    unsigned char* buffer = new unsigned char[this->mBSPHeader.ReadMatGroup.size + this->mBSPHeader.BVertex.size + this->mBSPHeader.WVertex.size + this->mBSPHeader.FVertex.size + this->mBSPHeader.Face.size + this->mBSPHeader.FaceId.size + this->mBSPHeader.VertexId.size];
    
    unsigned char* ptr = buffer;
    fread(ptr, this->mBSPHeader.BVertex.size, 1, Stream);
    char (*pBV)[3] = (char (*)[3])ptr; ptr += this->mBSPHeader.BVertex.size;

    fread(ptr, this->mBSPHeader.WVertex.size, 1, Stream);
    short (*pWV)[3] = (short (*)[3])ptr; ptr += this->mBSPHeader.WVertex.size;

    fread(ptr, this->mBSPHeader.FVertex.size, 1, Stream);
    float (*pFV)[3] = (float (*)[3])ptr; ptr += this->mBSPHeader.FVertex.size;

    fread(ptr, this->mBSPHeader.Face.size, 1, Stream);
    _BSP_READ_FACE *pRF = (_BSP_READ_FACE *)ptr; ptr += this->mBSPHeader.Face.size;

    fread(ptr, this->mBSPHeader.FaceId.size, 1, Stream);
    unsigned int *pFI = (unsigned int *)ptr; ptr += this->mBSPHeader.FaceId.size;

    fread(ptr, this->mBSPHeader.VertexId.size, 1, Stream);
    unsigned int *pVI = (unsigned int *)ptr; ptr += this->mBSPHeader.VertexId.size;

    fread(ptr, this->mBSPHeader.ReadMatGroup.size, 1, Stream);
    _BSP_READ_M_GROUP *pRM = (_BSP_READ_M_GROUP *)ptr;

    if (IsServerMode())
    {
        this->OnlyStoreCollisionStructure(pRM, pBV, pWV, pFV, pVI, pRF, pFI);
    }
    
    delete[] buffer;
}

void CBsp::ReadDynamicDataExtBsp(FILE *Stream) {}
