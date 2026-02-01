#include "pch.h"

#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CMainThread.h"

#include <cmath>
#include <xmmintrin.h>

float DotProduct(const float *a1, const float *a2)
{
    return (a1[1] * a2[1]) + (*a1 * *a2) + (a1[2] * a2[2]);
}

void GetNormal(float *const a1, const float *const a2, const float *const a3, const float *const a4)
{
    float v6 = a4[2] - a2[2];
    float v7 = a3[2] - a2[2];
    float v8 = a3[1] - a2[1];
    float v9 = *a3 - *a2;
    float v10 = *a4 - *a2;
    float v11 = a4[1] - a2[1];

    float v12 = (v7 - v6) * 0.0f + (v6 - 0.0f) * v8 + (0.0f - v7) * v11; // Simplified: Cross product
    float v13 = (v9 - v10) * 0.0f + (v10 - 0.0f) * v7 + (0.0f - v9) * v6;
    float v14 = (v8 - v11) * 0.0f + (v11 - 0.0f) * v9 + (0.0f - v8) * v10;

    // Direct cross product (a3-a2) x (a4-a2)
    float ux = a3[0] - a2[0], uy = a3[1] - a2[1], uz = a3[2] - a2[2];
    float vx = a4[0] - a2[0], vy = a4[1] - a2[1], vz = a4[2] - a2[2];
    
    a1[0] = uy * vz - uz * vy;
    a1[1] = uz * vx - ux * vz;
    a1[2] = ux * vy - uy * vx;

    float mag = sqrtf(a1[0]*a1[0] + a1[1]*a1[1] + a1[2]*a1[2]);
    if (mag > 0.00001f) {
        a1[0] /= mag;
        a1[1] /= mag;
        a1[2] /= mag;
    }
    a1[3] = (a1[0] * a2[0]) + (a1[1] * a2[1]) + (a1[2] * a2[2]);
}

int GetPlaneCrossPoint(const float *const a1, const float *const a2, float *const a3, const float *const a4, float a5)
{
    float v6 = (a4[0] * a1[0] + a4[1] * a1[1] + a4[2] * a1[2]) - a5;
    float v7 = (a4[0] * a2[0] + a4[1] * a2[1] + a4[2] * a2[2]) - a5;
    
    if ((v6 < 0.0f && v7 > 0.0f) || (v6 >= 0.0f && v7 < 0.0f))
    {
        float v9 = v6 / (v6 - v7);
        a3[0] = (a2[0] - a1[0]) * v9 + a1[0];
        a3[1] = (a2[1] - a1[1]) * v9 + a1[1];
        a3[2] = (a2[2] - a1[2]) * v9 + a1[2];
        return 1;
    }
    return 0;
}

void CrossProduct(const float *a1, const float *a2, float *a3)
{
    a3[0] = (a2[2] * a1[1]) - (a2[1] * a1[2]);
    a3[1] = (*a2 * a1[2]) - (*a1 * a2[2]);
    a3[2] = (*a1 * a2[1]) - (*a2 * a1[1]);
}

bool CheckEdgeEpsilon(const float *const a1, const float *const a2, const float *const a3, const float *const a4)
{
    float ux = a1[0] - a2[0], uy = a1[1] - a2[1], uz = a1[2] - a2[2];
    float cross[3];
    cross[0] = (ux * a4[2]) - (uz * a4[1]); // This depends on cross product implementation details in IDA
    // Looking at int_CheckEdgeEpsilon: sub_1404E2FB0(a4, v11, v12);
    // where v11 is a1 - a2.
    // So it's cross(a4, a1-a2).
    
    float crossSource[3] = {ux, uy, uz};
    CrossProduct(a4, crossSource, cross);
    
    // dot(a3 - a2, cross)
    float vx = a3[0] - a2[0], vy = a3[1] - a2[1], vz = a3[2] - a2[2];
    return (vx * cross[0] + vy * cross[1] + vz * cross[2]) <= 0.01f;
}

void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3)
{
    *a1 = (float)((float)((float)a2[0] / 127.0f) * a3->scale) + a3->pos[0];
    a1[1] = (float)((float)((float)a2[1] / 127.0f) * a3->scale) + a3->pos[1];
    a1[2] = (float)((float)((float)a2[2] / 127.0f) * a3->scale) + a3->pos[2];
}

void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3)
{
    *a1 = (float)((float)((float)a2[0] / 32767.0f) * a3->scale) + a3->pos[0];
    a1[1] = (float)((float)((float)a2[1] / 32767.0f) * a3->scale) + a3->pos[1];
    a1[2] = (float)((float)((float)a2[2] / 32767.0f) * a3->scale) + a3->pos[2];
}

void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3)
{
    *a1 = a2[0];
    a1[1] = a2[1];
    a1[2] = a2[2];
}

float GetFloatMod(float a1, float a2)
{
    float v2 = a1;
    if (a2 == 0.0f)
        return 0.0f;
    if (a1 < 0.0f)
        v2 = -a1;
    for (; v2 >= 32768.0f; v2 -= 32768.0f)
        ;
    return (static_cast<int>(v2 * 32768.0f) % static_cast<int>(a2 * 32768.0f)) * 0.000030517578f;
}

void MatrixIdentity(float (*const a1)[4])
{
    memset_0(a1, 0, 0x40u);
    (*a1)[0] = 1.0f;
    (*a1)[5] = 1.0f;
    (*a1)[10] = 1.0f;
    (*a1)[15] = 1.0f;
}

void MatrixCopy(float (*const a1)[4], float (*const a2)[4])
{
    memcpy_0(a1, a2, 0x40u);
}

void MatrixMultiply(float (*const a1)[4], float (*const a2)[4], float (*const a3)[4])
{
    float *v3 = &(*a3)[8];
    float *v4 = &(*a2)[2];
    __int64 v5 = 4;
    do
    {
        float *v6 = v3;
        __int64 v7 = 4;
        do
        {
            float v8 = *(v6 - 8);
            a1 = reinterpret_cast<float (*)[4]>(reinterpret_cast<char *>(a1) + 4);
            ++v6;
            --v7;
            float v9 = (v8 * *(v4 - 2)) + (*a1)[-1];
            (*a1)[-1] = v9;
            float v10 = (*(v6 - 5) * *(v4 - 1)) + v9;
            (*a1)[-1] = v10;
            float v11 = (*(v6 - 1) * *v4) + v10;
            (*a1)[-1] = v11;
            (*a1)[-1] = (v6[3] * v4[1]) + v11;
        } while (v7);
        v4 += 4;
        --v5;
    } while (v5);
}

int MatrixInvert(float (*const a1)[4], float (*const a2)[4])
{
    if (std::fabs((*a2)[15] - 1.0f) > 0.001f)
        return 0;
    if (std::fabs((*a2)[3]) > 0.001f)
        return 0;
    if (std::fabs((*a2)[7]) > 0.001f)
        return 0;
    if (std::fabs((*a2)[11]) > 0.001f)
        return 0;

    float v6 = (*a2)[6];
    float v7 = (*a2)[5];
    float v8 = (v7 * (*a2)[10]) - (v6 * (*a2)[9]);
    float v9 = 1.0f
             / ((v8 * (*a2)[0])
                - (((*a2)[4] * (*a2)[10]) - ((*a2)[8] * v6)) * (*a2)[1]
                + (((*a2)[4] * (*a2)[9]) - ((*a2)[8] * v7)) * (*a2)[2]);
    float v10 = v8 * v9;
    (*a1)[0] = v10;
    float v11 = -(((((*a2)[1] * (*a2)[10]) - ((*a2)[2] * (*a2)[9])) * v9));
    (*a1)[1] = v11;
    float v12 = (*a2)[2] * (*a2)[5];
    float v13 = (*a2)[1] * (*a2)[6];
    (*a1)[3] = 0.0f;
    float v14 = (v13 - v12) * v9;
    (*a1)[2] = v14;
    float v15 = -(((((*a2)[10] * (*a2)[4]) - ((*a2)[6] * (*a2)[8])) * v9));
    (*a1)[4] = v15;
    float v16 = (((*a2)[10] * (*a2)[0]) - ((*a2)[2] * (*a2)[8])) * v9;
    (*a1)[5] = v16;
    float v17 = (*a2)[2] * (*a2)[4];
    float v18 = (*a2)[6] * (*a2)[0];
    (*a1)[7] = 0.0f;
    float v19 = -((v18 - v17) * v9);
    (*a1)[6] = v19;
    float v20 = (((*a2)[9] * (*a2)[4]) - ((*a2)[5] * (*a2)[8])) * v9;
    (*a1)[8] = v20;
    float v21 = -(((((*a2)[0] * (*a2)[9]) - ((*a2)[1] * (*a2)[8])) * v9));
    (*a1)[9] = v21;
    float v22 = (*a2)[1] * (*a2)[4];
    float v23 = (*a2)[5] * (*a2)[0];
    (*a1)[11] = 0.0f;
    float v25 = (v23 - v22) * v9;
    (*a1)[10] = v25;
    (*a1)[12] = -((v15 * (*a2)[13]) + ((*a2)[12] * v10) + (v20 * (*a2)[14]));
    (*a1)[13] = -(((*a2)[12] * v11) + ((*a2)[13] * v16) + (v21 * (*a2)[14]));
    float v26 = (*a2)[13];
    float v27 = (*a2)[12];
    float v28 = v25 * (*a2)[14];
    (*a1)[15] = 1.0f;
    (*a1)[14] = -((v26 * v19) + (v27 * v14) + v28);
    return 1;
}

void MatrixFromQuaternion(float (*const a1)[4], float a2, float a3, float a4, float a5)
{
    (*a1)[15] = 1.0f;
    (*a1)[11] = 0.0f;
    (*a1)[7] = 0.0f;
    (*a1)[3] = 0.0f;
    (*a1)[14] = 0.0f;
    (*a1)[13] = 0.0f;
    (*a1)[12] = 0.0f;
    (*a1)[0] = 1.0f - (((a4 * a4) + (a3 * a3)) * 2.0f);
    (*a1)[1] = ((a2 * a3) - (a4 * a5)) * 2.0f;
    (*a1)[2] = ((a3 * a5) + (a2 * a4)) * 2.0f;
    (*a1)[4] = ((a4 * a5) + (a2 * a3)) * 2.0f;
    (*a1)[9] = ((a2 * a5) + (a3 * a4)) * 2.0f;
    (*a1)[8] = ((a2 * a4) - (a3 * a5)) * 2.0f;
    (*a1)[5] = 1.0f - (((a4 * a4) + (a2 * a2)) * 2.0f);
    (*a1)[6] = ((a3 * a4) - (a2 * a5)) * 2.0f;
    (*a1)[10] = 1.0f - (((a3 * a3) + (a2 * a2)) * 2.0f);
}

void QuaternionSlerp(
    float *a1,
    float *a2,
    float *a3,
    float *a4,
    float a5,
    float a6,
    float a7,
    float a8,
    float a9,
    float a10,
    float a11,
    float a12,
    float a13)
{
    float v13 = a5;
    float v18 = a6;
    float v19 = a7;
    float v20 = a8;
    float v21 = a9;
    float v22 = a11;
    float v23 = (((a5 * a9) + (a6 * a10)) + (a7 * a11)) + (a8 * a12);
    if (v23 < 0.0f)
    {
        v13 = -a5;
        v18 = -a6;
        v19 = -a7;
        v20 = -a8;
        v23 = -v23;
    }
    float v24 = 0.0f;
    float v25 = 0.0f;
    if ((v23 + 1.0f) <= 0.050000001f)
    {
        a10 = v13;
        a12 = v19;
        v21 = -v18;
        v22 = -v20;
        v25 = static_cast<float>(sin_0((0.5f - a13) * 3.141592f));
        v24 = static_cast<float>(sin_0(a13 * 3.141592f));
    }
    else if ((1.0f - v23) >= 0.050000001f)
    {
        float v26 = acosf_0(v23);
        float v27 = sinf_0(v26);
        v25 = sinf_0((1.0f - a13) * v26) / v27;
        v24 = sinf_0(v26 * a13) / v27;
    }
    else
    {
        v24 = a13;
        v25 = 1.0f - a13;
    }
    *a1 = (v25 * v13) + (v24 * v21);
    *a2 = (v25 * v18) + (v24 * a10);
    *a3 = (v25 * v19) + (v24 * v22);
    *a4 = (v25 * v20) + (v24 * a12);
}

void GetMatrixFrom3DSMAXMatrix(float (*const a1)[4])
{
    float v1 = (*a1)[1];
    float v2 = (*a1)[5];
    (*a1)[1] = (*a1)[2];
    float v3 = (*a1)[6];
    (*a1)[2] = v1;
    float v4 = (*a1)[9];
    (*a1)[6] = v2;
    (*a1)[5] = v3;
    float v5 = (*a1)[13];
    (*a1)[9] = (*a1)[10];
    float v6 = (*a1)[14];
    (*a1)[10] = v4;
    (*a1)[14] = v5;
    (*a1)[13] = v6;
}

static __m128 sub_14051F9D0(int a1, float *a2, int a3, double a4)
{
    int v4 = a1 - 1;
    int v5 = 0;
    if (a1 - 1 > 0)
    {
        __int64 v6 = a3;
        float *v7 = a2;
        unsigned __int64 v8 = 4ULL * a3;
        float *v9 = &a2[v8 / 4];
        while (*(float *)&a4 < *v7 || *v9 <= *(float *)&a4)
        {
            ++v5;
            v7 = (float *)((char *)v7 + v8);
            v9 = (float *)((char *)v9 + v8);
            if (v5 >= v4)
                goto LABEL_6;
        }
        dword_184A893FC = v5;
        dword_184A89400 = v5 + 1;
        if (v5 == -1)
        {
LABEL_6:
            dword_184A893FC = v4;
            dword_184A89400 = v4;
            return _mm_setzero_ps();
        }
        float v11 = a2[v6 * v5];
        *(float *)&a4 = (float)(*(float *)&a4 - v11) / (float)(a2[v6 * (v5 + 1)] - v11);
        return _mm_set_ps1(*(float *)&a4);
    }
LABEL_6:
    dword_184A893FC = v4;
    dword_184A89400 = v4;
    return _mm_setzero_ps();
}

void GetAniMatrix(float (*const a1)[4], _ANI_OBJECT *a2, double a3)
{
    float v17 = 0.0f;
    float v18[3]{};
    float v19[16]{};
    float v20[16]{};
    float v21[16]{};
    float v22[16]{};
    float v23 = 0.0f;
    float v24 = 0.0f;

    MatrixIdentity(a1);
    if (!*reinterpret_cast<unsigned long long *>(&a2->Pos_cnt) && !a2->Scale_cnt)
    {
        MatrixFromQuaternion(a1, a2->quat[0], a2->quat[1], a2->quat[2], a2->quat[3]);
        MatrixMultiply(a1, a1, a2->s_matrix);
        (*a1)[12] = a2->pos[0];
        (*a1)[13] = a2->pos[1];
        (*a1)[14] = a2->pos[2];
        return;
    }

    int Rot_cnt = a2->Rot_cnt;
    if (Rot_cnt)
    {
        __m128 v6 = sub_14051F9D0(Rot_cnt, &a2->Rot->frame, 5, a3);
        QuaternionSlerp(
            v18,
            &v17,
            &v24,
            &v23,
            a2->Rot[dword_184A893FC].quat[0],
            a2->Rot[dword_184A893FC].quat[1],
            a2->Rot[dword_184A893FC].quat[2],
            a2->Rot[dword_184A893FC].quat[3],
            a2->Rot[dword_184A89400].quat[0],
            a2->Rot[dword_184A89400].quat[1],
            a2->Rot[dword_184A89400].quat[2],
            a2->Rot[dword_184A89400].quat[3],
            _mm_cvtss_f32(v6));
        MatrixFromQuaternion(a1, v18[0], v17, v24, v23);
    }
    else
    {
        MatrixFromQuaternion(a1, a2->quat[0], a2->quat[1], a2->quat[2], a2->quat[3]);
    }

    int Scale_cnt = a2->Scale_cnt;
    if (Scale_cnt)
    {
        __m128 v9 = sub_14051F9D0(Scale_cnt, &a2->Scale->frame, 8, a3);
        MatrixIdentity(reinterpret_cast<float (*)[4]>(v19));
        _SCALE_TRACK *Scale = a2->Scale;
        __int64 v11 = dword_184A893FC;
        __int64 v12 = dword_184A89400;
        v19[0] = ((Scale[v12].scale[0] - Scale[v11].scale[0]) * _mm_cvtss_f32(v9)) + Scale[v11].scale[0];
        v19[5] = ((Scale[v12].scale[1] - Scale[v11].scale[1]) * _mm_cvtss_f32(v9)) + Scale[v11].scale[1];
        v19[10] = ((Scale[v12].scale[2] - Scale[v11].scale[2]) * _mm_cvtss_f32(v9)) + Scale[v11].scale[2];
        QuaternionSlerp(
            v18,
            &v17,
            &v24,
            &v23,
            Scale[v11].scale_axis[0],
            Scale[v11].scale_axis[1],
            Scale[v11].scale_axis[2],
            Scale[v11].scale_axis[3],
            Scale[v12].scale_axis[0],
            Scale[v12].scale_axis[1],
            Scale[v12].scale_axis[2],
            Scale[v12].scale_axis[3],
            _mm_cvtss_f32(v9));
        MatrixFromQuaternion(reinterpret_cast<float (*)[4]>(v21), v18[0], v17, v24, v23);
        MatrixInvert(reinterpret_cast<float (*)[4]>(v22), reinterpret_cast<float (*)[4]>(v21));
        MatrixMultiply(reinterpret_cast<float (*)[4]>(v20), reinterpret_cast<float (*)[4]>(v19), reinterpret_cast<float (*)[4]>(v22));
        MatrixMultiply(reinterpret_cast<float (*)[4]>(v20), reinterpret_cast<float (*)[4]>(v21), reinterpret_cast<float (*)[4]>(v20));
        MatrixMultiply(a1, a1, reinterpret_cast<float (*)[4]>(v20));
    }
    else
    {
        MatrixMultiply(a1, a1, a2->s_matrix);
    }

    int Pos_cnt = a2->Pos_cnt;
    if (!Pos_cnt)
    {
        (*a1)[12] = a2->pos[0];
        (*a1)[13] = a2->pos[1];
        (*a1)[14] = a2->pos[2];
        return;
    }
    __m128 v14 = sub_14051F9D0(Pos_cnt, &a2->Pos->frame, 4, a3);
    __int64 v15 = dword_184A89400;
    __int64 v16 = dword_184A893FC;
    (*a1)[12] = ((a2->Pos[dword_184A89400].pos[0] - a2->Pos[dword_184A893FC].pos[0]) * _mm_cvtss_f32(v14))
              + a2->Pos[dword_184A893FC].pos[0];
    (*a1)[13] = ((a2->Pos[v15].pos[1] - a2->Pos[v16].pos[1]) * _mm_cvtss_f32(v14)) + a2->Pos[v16].pos[1];
    (*a1)[14] = ((a2->Pos[v15].pos[2] - a2->Pos[v16].pos[2]) * _mm_cvtss_f32(v14)) + a2->Pos[v16].pos[2];
}

static void sub_14051FF20(float (*a1)[4], unsigned short a2, __int64 a3, double a4)
{
    float v7[16]{};
    float v8[16]{};
    if (a2)
    {
        sub_14051FF20(reinterpret_cast<float (*)[4]>(v8), *reinterpret_cast<unsigned short *>(361LL * a2 + a3 - 231), a3, a4);
        GetAniMatrix(reinterpret_cast<float (*)[4]>(v7), reinterpret_cast<_ANI_OBJECT *>(a3 + 361LL * (a2 - 1)), a4);
        MatrixMultiply(a1, reinterpret_cast<float (*)[4]>(v8), reinterpret_cast<float (*)[4]>(v7));
    }
    else
    {
        MatrixIdentity(a1);
    }
}

void GetObjectMatrix(float (*const f_matrix)[4], unsigned short a2, _ANI_OBJECT *a3, double a4)
{
    double v4 = 0.0;
    double v5 = a4;
    float *v8 = (float *)f_matrix;
    double v9 = 0.0;
    if (a2)
    {
        __int64 v10 = a2;
        float *v11 = nullptr;
        if (a3[v10 - 1].AniFrameCache)
        {
            v11 = a3[a2 - 1].f_matrix[0];
        }
        else
        {
            unsigned short parent = a3[v10 - 1].parent;
            a3[v10 - 1].AniFrameCache = 1;
            if (parent)
            {
                int frames = a3[parent - 1].frames;
                if (frames)
                    v9 = GetFloatMod(static_cast<float>(a4), static_cast<float>(frames));
            }
            int v15 = a3[v10 - 1].frames;
            if (v15)
                v4 = GetFloatMod(static_cast<float>(v5), static_cast<float>(v15));
            float v19[16]{};
            float v18[16]{};
            sub_14051FF20(reinterpret_cast<float (*)[4]>(v19), parent, reinterpret_cast<__int64>(a3), v9);
            _ANI_OBJECT *v17 = &a3[a2 - 1];
            GetAniMatrix(reinterpret_cast<float (*)[4]>(v18), v17, v4);
            MatrixMultiply(reinterpret_cast<float (*)[4]>(v8), reinterpret_cast<float (*)[4]>(v19), reinterpret_cast<float (*)[4]>(v18));
            GetMatrixFrom3DSMAXMatrix(reinterpret_cast<float (*)[4]>(v8));
            f_matrix = v17->f_matrix;
            v11 = v8;
        }
        MatrixCopy(f_matrix, reinterpret_cast<float (*)[4]>(v11));
    }
    else
    {
        MatrixIdentity(f_matrix);
    }
}
