#include "pch.h"

#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CMainThread.h"

#include <cmath>
#include <xmmintrin.h>
#include <intrin.h>

float DotProduct(const float *a1, const float *a2)
{
    return (a1[1] * a2[1]) + (*a1 * *a2) + (a1[2] * a2[2]);
}

float GetSqrt(float *fPos, float *fTar)
{
    float dx = fTar[0] - fPos[0];
    float dz = fTar[2] - fPos[2];
    return std::sqrt((dx * dx) + (dz * dz));
}

float GetDist(float *const a1, float *const a2)
{
    const float dx = *a2 - *a1;
    const float dz = a2[2] - a1[2];
    return sqrtf_0((dx * dx) + (dz * dz));
}

float Get3DSqrt(float *Pos, float *Tar)
{

    const float dx = *Pos - *Tar;
    const float dy = Pos[1] - Tar[1];
    const float dz = Pos[2] - Tar[2];
    return sqrtf_0((dx * dx) + (dy * dy) + (dz * dz));
}

float GetYAngle(float *Pos, float *Tar)
{

    const float dx = *Tar - *Pos;
    const float dz = Tar[2] - Pos[2];
    const float dist = sqrtf_0((dx * dx) + (dz * dz));
    if (dist == 0.0f)
    {
        return 0.0f;
    }

    const double angle = acos_0(dz / dist);
    if (dx <= 0.0f)
    {
        return static_cast<float>((-angle) * 65535.0 / 6.283185307 + 32768.0);
    }
    return static_cast<float>(angle * 65535.0 / 6.283185307 + 32768.0);
}

void Normalize(float *v)
{

    const float length = sqrtf_0((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
    const double len = length;
    v[0] = v[0] / static_cast<float>(len);
    v[1] = v[1] / static_cast<float>(len);
    v[2] = v[2] / static_cast<float>(len);
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

void sub_1404E2FB0(float *a1, float *a2, float *a3)
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

bool IsCollisionBBoxPoint(float *const a1, float *const a2, float *const a3)
{
    if (*a3 >= *a1 && *a2 >= *a3)
    {
        float y = a3[1];
        if (y >= a1[1] && a2[1] >= y)
        {
            float z = a3[2];
            if (z >= a1[2] && a2[2] >= z)
            {
                return true;
            }
        }
    }
    return false;
}

bool IsCollisionBBoxPoint(short *const a1, short *const a2, float *const a3)
{
    if (*a3 >= static_cast<float>(a1[0]) && static_cast<float>(a2[0]) >= *a3)
    {
        float y = a3[1];
        if (y >= static_cast<float>(a1[1]) && static_cast<float>(a2[1]) >= y)
        {
            float z = a3[2];
            if (z >= static_cast<float>(a1[2]) && static_cast<float>(a2[2]) >= z)
            {
                return true;
            }
        }
    }
    return false;
}

void sub_14050C650(float *a1, float *a2, float *a3)
{
    float v3 = *a2 - *a3;
    float v5 = a2[1] - a3[1];
    float v6 = a2[2] - a3[2];
    float v7 = sqrtf_0((v5 * v5) + (v3 * v3) + (v6 * v6));
    a1[0] = (v3 / v7) * 2.0f + a1[0];
    a1[1] = (v5 / v7) * 2.0f + a1[1];
    a1[2] = (v6 / v7) * 2.0f + a1[2];
}

unsigned int sub_1404E1570(float a1, float a2, float a3)
{
    const float bestHeight = a1 + a2;
    if (a3 < a1)
    {
        return static_cast<unsigned int>(2 * static_cast<int>((a1 - a3) / (bestHeight - a1)) + 1);
    }
    return static_cast<unsigned int>(2 * static_cast<int>((a3 - a1) / (bestHeight - a1)));
}

unsigned int sub_1404E46E0(float a1, float a2, float a3)
{
    const float bestHeight = a1 + a2;
    if (a3 < a1)
    {
        return static_cast<unsigned int>(2 * static_cast<int>((a1 - a3) / (bestHeight - a1)) + 1);
    }
    return static_cast<unsigned int>(2 * static_cast<int>((a3 - a1) / (bestHeight - a1)));
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

void MatrixMultiply(float (*a1)[4], float (*const a2)[4], float (*const a3)[4])
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

void MatrixScale(float (*const a1)[4], float a2, float a3, float a4)
{
    (*a1)[0] = a2;
    (*a1)[5] = a3;
    (*a1)[15] = 1.0f;
    (*a1)[1] = 0.0f;
    (*a1)[2] = 0.0f;
    (*a1)[10] = a4;
    (*a1)[3] = 0.0f;
    (*a1)[4] = 0.0f;
    (*a1)[6] = 0.0f;
    (*a1)[7] = 0.0f;
    (*a1)[8] = 0.0f;
    (*a1)[9] = 0.0f;
    (*a1)[11] = 0.0f;
    (*a1)[12] = 0.0f;
    (*a1)[13] = 0.0f;
    (*a1)[14] = 0.0f;
}

void MatrixRotate(float (*const a1)[4], float a2, float a3, float a4)
{
    const double v5 = a3 * 6.283184 / 360.0;
    const double v6 = a2 * 6.283184 / 360.0;
    const double v7 = a4 * 6.283184 / 360.0;

    float v16[4]{};
    int v17 = 0;
    float v18 = FLOAT_1_0;
    int v19 = 0;
    int v20 = 0;
    float v21 = 0.0f;
    int v22 = 0;
    float v23 = 0.0f;
    int v24 = 0;
    int v25 = 0;
    int v26 = 0;
    int v27 = 0;
    float v28 = FLOAT_1_0;
    float v29[16]{};
    float v30[44]{};

    float v8 = static_cast<float>(cos_0(v5));
    v16[0] = v8;
    v16[1] = 0.0f;
    v16[2] = -static_cast<float>(sin_0(v5));
    v16[3] = 0.0f;
    v17 = 0;
    v18 = FLOAT_1_0;
    v19 = 0;
    v20 = 0;
    v21 = static_cast<float>(sin_0(v5));
    v22 = 0;
    v23 = v8;
    v24 = 0;
    v25 = 0;
    v26 = 0;
    v27 = 0;
    v28 = FLOAT_1_0;

    v29[0] = FLOAT_1_0;
    v29[1] = 0.0f;
    v29[2] = 0.0f;
    v29[3] = 0.0f;
    v29[4] = 0.0f;
    const float cos_v6 = static_cast<float>(cos_0(v6));
    v29[5] = cos_v6;
    const float sin_v6 = static_cast<float>(sin_0(v6));
    v29[7] = 0.0f;
    v29[8] = 0.0f;
    v29[10] = cos_v6;
    v29[11] = 0.0f;
    v29[12] = 0.0f;
    v29[6] = sin_v6;
    v29[13] = 0.0f;
    v29[14] = 0.0f;
    v29[15] = FLOAT_1_0;
    v29[9] = -sin_v6;

    MatrixMultiply(reinterpret_cast<float (*)[4]>(v30), reinterpret_cast<float (*)[4]>(v16), reinterpret_cast<float (*)[4]>(v29));

    const float cos_v7 = static_cast<float>(cos_0(v7));
    v16[0] = cos_v7;
    v16[1] = static_cast<float>(sin_0(v7));
    v16[2] = 0.0f;
    v16[3] = 0.0f;
    float neg_sin_v7 = -static_cast<float>(sin_0(v7));
    v17 = *reinterpret_cast<int *>(&neg_sin_v7);
    v18 = cos_v7;
    v19 = 0;
    v20 = 0;
    v21 = 0.0f;
    v22 = 0;
    v23 = FLOAT_1_0;
    v24 = 0;
    v25 = 0;
    v26 = 0;
    v27 = 0;
    v28 = FLOAT_1_0;

    MatrixMultiply(a1, reinterpret_cast<float (*)[4]>(v16), reinterpret_cast<float (*)[4]>(v30));
}

void MatrixRotateX(float (*const a1)[4], float a2)
{
    const double v3 = a2 * 6.283184 / 360.0;
    const float v4 = static_cast<float>(cos_0(v3));
    const float v6 = static_cast<float>(sin_0(v3));

    (*a1)[0] = 1.0f;
    (*a1)[1] = 0.0f;
    (*a1)[2] = 0.0f;
    (*a1)[3] = 0.0f;
    (*a1)[4] = 0.0f;
    (*a1)[5] = v4;
    (*a1)[10] = v4;
    (*a1)[7] = 0.0f;
    (*a1)[8] = 0.0f;
    (*a1)[11] = 0.0f;
    (*a1)[12] = 0.0f;
    (*a1)[13] = 0.0f;
    (*a1)[6] = v6;
    (*a1)[14] = 0.0f;
    (*a1)[15] = 1.0f;
    (*a1)[9] = -v6;
}

void MatrixRotateY(float (*const a1)[4], float a2)
{
    const double v3 = a2 * 6.283184 / 360.0;
    const float v5 = static_cast<float>(cos_0(v3));
    const float v6 = static_cast<float>(sin_0(v3));

    (*a1)[1] = 0.0f;
    (*a1)[0] = v5;
    (*a1)[3] = 0.0f;
    (*a1)[4] = 0.0f;
    (*a1)[5] = 1.0f;
    (*a1)[10] = v5;
    (*a1)[6] = 0.0f;
    (*a1)[7] = 0.0f;
    (*a1)[9] = 0.0f;
    (*a1)[11] = 0.0f;
    (*a1)[12] = 0.0f;
    (*a1)[13] = 0.0f;
    (*a1)[8] = v6;
    (*a1)[14] = 0.0f;
    (*a1)[15] = 1.0f;
    (*a1)[2] = -v6;
}

void MatrixRotateZ(float (*const a1)[4], float a2)
{
    const double v3 = a2 * 6.283184 / 360.0;
    const float v4 = static_cast<float>(cos_0(v3));
    const float v6 = static_cast<float>(sin_0(v3));

    (*a1)[0] = v4;
    (*a1)[5] = v4;
    (*a1)[10] = 1.0f;
    (*a1)[2] = 0.0f;
    (*a1)[3] = 0.0f;
    (*a1)[6] = 0.0f;
    (*a1)[7] = 0.0f;
    (*a1)[8] = 0.0f;
    (*a1)[1] = v6;
    (*a1)[9] = 0.0f;
    (*a1)[11] = 0.0f;
    (*a1)[12] = 0.0f;
    (*a1)[13] = 0.0f;
    (*a1)[14] = 0.0f;
    (*a1)[4] = -v6;
    (*a1)[15] = 1.0f;
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

void GetObjectMatrix(float (*f_matrix)[4], unsigned short a2, _ANI_OBJECT *a3, double a4)
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

void ConvAniObject(int a1, unsigned __int8 *a2, _READ_ANI_OBJECT *a3, _ANI_OBJECT *a4)
{
    if (a1 <= 0)
        return;

    int *p_frames = &a4->frames;
    char *v6 = reinterpret_cast<char *>(a3) + 4;
    float *v7 = a4->s_matrix[0];
    unsigned int v8 = static_cast<unsigned int>(a1);

    do
    {
        *reinterpret_cast<unsigned short *>(p_frames - 2) = *reinterpret_cast<unsigned short *>(v6 - 2);
        *p_frames = *reinterpret_cast<int *>(v6);
        *reinterpret_cast<unsigned short *>(p_frames - 1) = *reinterpret_cast<unsigned short *>(v6 - 1);
        p_frames[1] = *reinterpret_cast<int *>(v6 + 4);
        p_frames[2] = *reinterpret_cast<int *>(v6 + 8);
        int v9 = *reinterpret_cast<int *>(v6 + 12);
        *reinterpret_cast<int *>(reinterpret_cast<char *>(p_frames) + 161) = 0;
        p_frames[3] = v9;
        _mm_prefetch(v6 + 556, 0);
        p_frames[7] = *reinterpret_cast<int *>(v6 + 28);
        p_frames[8] = *reinterpret_cast<int *>(v6 + 32);
        p_frames[9] = *reinterpret_cast<int *>(v6 + 36);
        p_frames[10] = *reinterpret_cast<int *>(v6 + 40);
        p_frames[4] = *reinterpret_cast<int *>(v6 + 16);
        p_frames[5] = *reinterpret_cast<int *>(v6 + 20);
        p_frames[6] = *reinterpret_cast<int *>(v6 + 24);
        p_frames[11] = *reinterpret_cast<int *>(v6 + 44);
        p_frames[12] = *reinterpret_cast<int *>(v6 + 48);
        p_frames[13] = *reinterpret_cast<int *>(v6 + 52);
        p_frames[14] = *reinterpret_cast<int *>(v6 + 56);
        p_frames[15] = *reinterpret_cast<int *>(v6 + 60);
        p_frames[16] = *reinterpret_cast<int *>(v6 + 64);
        p_frames[17] = *reinterpret_cast<int *>(v6 + 68);
        *reinterpret_cast<unsigned long long *>(p_frames + 18) = reinterpret_cast<unsigned long long>(&a2[*reinterpret_cast<int *>(v6 + 80)]);
        *reinterpret_cast<unsigned long long *>(p_frames + 20) = reinterpret_cast<unsigned long long>(&a2[*reinterpret_cast<int *>(v6 + 72)]);
        *reinterpret_cast<unsigned long long *>(p_frames + 22) = reinterpret_cast<unsigned long long>(&a2[*reinterpret_cast<int *>(v6 + 76)]);

        float v16[16]{};
        float v17[16]{};
        float v18[16]{};
        float v19[16]{};

        MatrixIdentity(reinterpret_cast<float (*)[4]>(v16));
        float v10 = *reinterpret_cast<float *>(v6 + 24);
        float v11 = *reinterpret_cast<float *>(v6 + 16);
        float v12 = *reinterpret_cast<float *>(v6 + 36);
        float v13 = *reinterpret_cast<float *>(v6 + 32);
        v16[5] = *reinterpret_cast<float *>(v6 + 20);
        float v14 = *reinterpret_cast<float *>(v6 + 40);
        v16[10] = v10;
        float v15 = *reinterpret_cast<float *>(v6 + 28);
        v16[0] = v11;
        MatrixFromQuaternion(reinterpret_cast<float (*)[4]>(v17), v15, v13, v12, v14);
        MatrixInvert(reinterpret_cast<float (*)[4]>(v18), reinterpret_cast<float (*)[4]>(v17));
        MatrixMultiply(reinterpret_cast<float (*)[4]>(v19), reinterpret_cast<float (*)[4]>(v16), reinterpret_cast<float (*)[4]>(v18));
        MatrixMultiply(reinterpret_cast<float (*)[4]>(v7), reinterpret_cast<float (*)[4]>(v17), reinterpret_cast<float (*)[4]>(v19));

        v7 = reinterpret_cast<float *>(reinterpret_cast<char *>(v7) + 361);
        p_frames = reinterpret_cast<int *>(reinterpret_cast<char *>(p_frames) + 361);
        v6 += 88;
        --v8;
    } while (v8);
}