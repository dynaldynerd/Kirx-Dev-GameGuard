#include "pch.h"
#include "CBsp.h"
#include "WorldServerUtil.h"
#include <cmath>
#include <cstdio>
#include <cstring>

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
    CBsp::WalkNodeForLeafList(this, 1, a2, a3, a5, &count);
    *a4 = count;
}

void CBsp::WalkNodeForLeafList(CBsp *pBsp, short nNodeId, float *const a3, float *const a4, short *a5, int *a6)
{
    if (nNodeId == 0) return;
    _BSP_NODE &node = pBsp->mNode[nNodeId];
    float *normal = pBsp->mCNNormal[node.f_normal_id];
    
    float v11 = DotProduct(normal, a3) - node.d;
    float v12 = DotProduct(normal, a4) - node.d;
    
    if ((v11 > 0.0f || v12 <= 0.0f) && (v12 > 0.0f || v11 <= 0.0f))
    {
        CBsp::SubLeafList(pBsp, v11, &node, a3, a4, a5, a6);
    }
    else
    {
        float v13 = v11 / (v11 - v12);
        float intersection[3];
        intersection[0] = (a4[0] - a3[0]) * v13 + a3[0];
        intersection[1] = (a4[1] - a3[1]) * v13 + a3[1];
        intersection[2] = (a4[2] - a3[2]) * v13 + a3[2];
        
        CBsp::SubLeafList(pBsp, v11, &node, a3, intersection, a5, a6);
        CBsp::SubLeafList(pBsp, v12, &node, intersection, a4, a5, a6);
    }
}

void CBsp::SubLeafList(CBsp *pBsp, float dist, _BSP_NODE *pNode, float *const a4, float *const a5, short *a6, int *a7)
{
    short next = (dist >= 0.0f) ? pNode->front : pNode->back;
    if (next < 0)
    {
        a6[(*a7)++] = -1 - next;
    }
    else
    {
        CBsp::WalkNodeForLeafList(pBsp, next, a4, a5, a6, a7);
    }
}

bool CBsp::LoadBsp(CBsp *pBsp, char *szFileName)
{
    FILE *Stream = nullptr;
    if (fopen_s(&Stream, szFileName, "rb") != 0 || !Stream) return false;

    fread(&pBsp->mBSPHeader, sizeof(_BSP_FILE_HEADER), 1, Stream);
    if (pBsp->mBSPHeader.version != 39) { fclose(Stream); return false; }

    pBsp->mLeafNum = pBsp->mBSPHeader.Leaf.size / (pBsp->mBSPHeader.version == 39 ? 0x10 : 1); // Mocking Leaf size if needed
    // The sizes in header are in bytes. 
    // From IDA: this->mLeafNum = (v6 + (((unsigned __int64)this->mBSPHeader.Leaf.size - v6) >> 1)) >> 4;
    // Which is roughly size / 0x10.
    pBsp->mLeafNum = pBsp->mBSPHeader.Leaf.size / 0x18; // Based on _BSP_LEAF size in IDA
    pBsp->mNodeNum = pBsp->mBSPHeader.Node.size / 0x18; 
    pBsp->mCFaceNum = pBsp->mBSPHeader.Face.size / 6;

    pBsp->mStaticAllocSize = pBsp->mBSPHeader.CPlanes.size + pBsp->mBSPHeader.CFaceId.size + pBsp->mBSPHeader.Node.size + pBsp->mBSPHeader.Leaf.size;
    pBsp->mStaticAlloc = new unsigned char[pBsp->mStaticAllocSize];
    
    unsigned char* ptr = pBsp->mStaticAlloc;
    pBsp->mCNNormal = (float (*)[3])ptr;
    fread(ptr, pBsp->mBSPHeader.CPlanes.size, 1, Stream);
    ptr += pBsp->mBSPHeader.CPlanes.size;

    pBsp->mCFaceId = (unsigned int*)ptr;
    fread(ptr, pBsp->mBSPHeader.CFaceId.size, 1, Stream);
    ptr += pBsp->mBSPHeader.CFaceId.size;

    pBsp->mNode = (_BSP_NODE*)ptr;
    fread(ptr, pBsp->mBSPHeader.Node.size, 1, Stream);
    ptr += pBsp->mBSPHeader.Node.size;

    pBsp->mLeaf = (_BSP_LEAF*)ptr;
    fread(ptr, pBsp->mBSPHeader.Leaf.size, 1, Stream);
    
    // Faces usually loaded in ReadDynamicDataFillVertexBuffer which builds collision structure for server
    pBsp->ReadDynamicDataFillVertexBuffer(pBsp, Stream);
    
    pBsp->mIsLoaded = 1;
    fclose(Stream);
    return true;
}

bool CBsp::LoadExtBsp(CBsp *pBsp, char *szFileName)
{
    pBsp->mIsLoadEBP = 1;
    return true;
}

bool CBsp::EdgeTest(CBsp *pBsp, float *pPos, unsigned int nLineId)
{
    _TOOL_COL_LINE &line = pBsp->mCFLine[nLineId];
    float *v = (float*)pBsp->mCFVertex;
    float *n = pBsp->mCFNormal[nLineId];

    float p1[3] = { v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2] };
    float p2[3] = { v[3 * line.start_v], v[3 * line.start_v + 1] + line.height, v[3 * line.start_v + 2] };

    if (!CheckEdgeEpsilon(p1, p2, pPos, n)) return false;

    float p3[3] = { v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p2, p3, pPos, n)) return false;

    float p4[3] = { v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p3, p4, pPos, n)) return false;

    return CheckEdgeEpsilon(p4, p1, pPos, n);
}

bool CBsp::CanYouGoThere(CBsp *pBsp, float *const a2, float *const a3, float (*a4)[3]) { 
    return !pBsp->mIsLoaded || CBsp::GetPathCrossPoint(pBsp, a2, a3, a4, 0, 0) == 0;
}

int CBsp::GetPathCrossPoint(CBsp *pBsp, float *const a2, float *const a3, float (*a4)[3], int a5, int a6)
{
    pBsp->mColFaceId = -1;
    if (pBsp->mCFLineNum < 2) return 0;

    float p1[3] = { a2[0], a2[1] + 16.0f, a2[2] };
    float p2[3] = { a3[0], a3[1] + 16.0f, a3[2] };

    short v66[32000];
    int v53 = 0;
    pBsp->GetLeafList(p1, p2, &v53, v66, 32000);

    float minDist = 100000.0f;
    int hit = 0;

    for (int i = 0; i < v53; ++i)
    {
        _TOOL_COL_LEAF &leaf = pBsp->mCFLeaf[v66[i]];
        for (unsigned int j = 0; j < leaf.line_num; ++j)
        {
            unsigned int lineIdx = pBsp->mCFLineId[leaf.start_id + j];
            if (lineIdx == a5 || lineIdx == a6) continue;

            float *n = pBsp->mCFNormal[lineIdx];
            float d1 = DotProduct(n, p1) - n[3];
            float d2 = DotProduct(n, p2) - n[3];

            if ((d1 > 0.0f && d2 > 0.0f) || (d1 < -0.01f && d2 < -0.01f)) continue;

            float t = d1 / (d1 - d2);
            float intersect[3] = { (p2[0] - p1[0]) * t + p1[0], (p2[1] - p1[1]) * t + p1[1], (p2[2] - p1[2]) * t + p1[2] };

            if (CBsp::EdgeTest(pBsp, intersect, lineIdx))
            {
                float dist = sqrtf((intersect[0] - p1[0]) * (intersect[0] - p1[0]) + (intersect[1] - p1[1]) * (intersect[1] - p1[1]) + (intersect[2] - p1[2]) * (intersect[2] - p1[2]));
                if (dist < minDist)
                {
                    minDist = dist;
                    pBsp->mColFaceId = lineIdx;
                    if (a4) memcpy(a4, intersect, 12);
                    hit = 1;
                }
            }
        }
    }

    if (hit) return 1;
    return 0;
}

void CBsp::CalcEntitiesMainColor(CBsp *pBsp) {}

void CBsp::SetCFNormal(CBsp *pBsp)
{
    if (pBsp->mCFLineNum)
    {
        for (unsigned int i = 0; i < pBsp->mCFLineNum; ++i)
        {
            _TOOL_COL_LINE &line = pBsp->mCFLine[i];
            float *v = (float*)pBsp->mCFVertex;
            float v20[3] = { v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2] };
            float v19[3] = { v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2] };
            float v18[3] = { v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2] };
            GetNormal(pBsp->mCFNormal[i], v20, v19, v18);
        }
    }
}

void CBsp::OnlyStoreCollisionStructure(CBsp *pBsp, _BSP_READ_M_GROUP *pRM, char (*pBV)[3], short (*pWV)[3], float (*pFV)[3], unsigned int *pVI, _BSP_READ_FACE *pRF, unsigned int *pFI)
{
    unsigned int vTotal = pBsp->mBSPHeader.FVertex.size / 12 + pBsp->mBSPHeader.BVertex.size / 3 + pBsp->mBSPHeader.WVertex.size / 6;
    pBsp->mCVertex = (float (*)[3])new float[vTotal * 3];
    pBsp->mCVertexId = new unsigned int[pBsp->mBSPHeader.VertexId.size / 4];
    pBsp->mCFace = new _BSP_C_FACE[pBsp->mCFaceNum];

    for (int i = 0; i < (int)(pBsp->mBSPHeader.ReadMatGroup.size / 0x2A); ++i)
    {
        _BSP_READ_M_GROUP &mGroup = pRM[i];
        unsigned char type = 4;
        if (mGroup.attr & 0x8000) type = 1; else if (mGroup.attr & 0x4000) type = 2;

        for (unsigned int j = 0; j < mGroup.face_num; ++j)
        {
            unsigned int faceIdx = pFI[j + mGroup.face_start_id];
            _BSP_READ_FACE &readFace = pRF[faceIdx];
            _BSP_C_FACE &face = pBsp->mCFace[faceIdx];
            
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

                memcpy(pBsp->mCVertex[vId], pos, 12);
                pBsp->mCVertexId[k + readFace.v_start_id] = vId;
            }

            if (readFace.v_num >= 3)
            {
                unsigned int v0 = pBsp->mCVertexId[readFace.v_start_id];
                unsigned int v1 = pBsp->mCVertexId[readFace.v_start_id + 1];
                unsigned int v2 = pBsp->mCVertexId[readFace.v_start_id + 2];
                GetNormal(face.Normal, pBsp->mCVertex[v0], pBsp->mCVertex[v1], pBsp->mCVertex[v2]);
            }
        }
    }
}

void CBsp::ReadDynamicDataFillVertexBuffer(CBsp *pBsp, FILE *Stream)
{
    unsigned char* buffer = new unsigned char[pBsp->mBSPHeader.ReadMatGroup.size + pBsp->mBSPHeader.BVertex.size + pBsp->mBSPHeader.WVertex.size + pBsp->mBSPHeader.FVertex.size + pBsp->mBSPHeader.Face.size + pBsp->mBSPHeader.FaceId.size + pBsp->mBSPHeader.VertexId.size];
    
    unsigned char* ptr = buffer;
    fread(ptr, pBsp->mBSPHeader.BVertex.size, 1, Stream);
    char (*pBV)[3] = (char (*)[3])ptr; ptr += pBsp->mBSPHeader.BVertex.size;

    fread(ptr, pBsp->mBSPHeader.WVertex.size, 1, Stream);
    short (*pWV)[3] = (short (*)[3])ptr; ptr += pBsp->mBSPHeader.WVertex.size;

    fread(ptr, pBsp->mBSPHeader.FVertex.size, 1, Stream);
    float (*pFV)[3] = (float (*)[3])ptr; ptr += pBsp->mBSPHeader.FVertex.size;

    fread(ptr, pBsp->mBSPHeader.Face.size, 1, Stream);
    _BSP_READ_FACE *pRF = (_BSP_READ_FACE *)ptr; ptr += pBsp->mBSPHeader.Face.size;

    fread(ptr, pBsp->mBSPHeader.FaceId.size, 1, Stream);
    unsigned int *pFI = (unsigned int *)ptr; ptr += pBsp->mBSPHeader.FaceId.size;

    fread(ptr, pBsp->mBSPHeader.VertexId.size, 1, Stream);
    unsigned int *pVI = (unsigned int *)ptr; ptr += pBsp->mBSPHeader.VertexId.size;

    fread(ptr, pBsp->mBSPHeader.ReadMatGroup.size, 1, Stream);
    _BSP_READ_M_GROUP *pRM = (_BSP_READ_M_GROUP *)ptr;

    if (IsServerMode())
    {
        CBsp::OnlyStoreCollisionStructure(pBsp, pRM, pBV, pWV, pFV, pVI, pRF, pFI);
    }
    
    delete[] buffer;
}

void CBsp::ReadDynamicDataExtBsp(CBsp *pBsp, FILE *Stream) {}
