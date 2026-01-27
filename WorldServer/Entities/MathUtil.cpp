#include "pch.h"
#include <cmath>

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
