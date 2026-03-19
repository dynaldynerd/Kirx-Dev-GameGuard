#include "pch.h"

#include "WorldServerUtil.h"
#include "CEntity.h"
#include "R3EngineGlobals.h"
#include "CMainThread.h"

#include <cmath>
#include <xmmintrin.h>
#include <intrin.h>

float DotProduct(const float *a1, const float *a2)
{
    return (a1[1] * a2[1]) + (*a1 * *a2) + (a1[2] * a2[2]);
}

static inline float SseLength2D(float dx, float dz)
{
    const __m128 diff = _mm_set_ps(0.0f, 0.0f, dz, dx);
    const __m128 sq = _mm_mul_ps(diff, diff);
    const __m128 dz2 = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 1, 1, 1));
    const __m128 sum = _mm_add_ss(sq, dz2);
    return _mm_cvtss_f32(_mm_sqrt_ss(sum));
}

static inline float SseLength3D(float dx, float dy, float dz)
{
    const __m128 diff = _mm_set_ps(0.0f, dz, dy, dx);
    const __m128 sq = _mm_mul_ps(diff, diff);
    const __m128 dy2 = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(1, 1, 1, 1));
    const __m128 dz2 = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 sum = _mm_add_ss(sq, dy2);
    sum = _mm_add_ss(sum, dz2);
    return _mm_cvtss_f32(_mm_sqrt_ss(sum));
}

float GetSqrt(float *fPos, float *fTar)
{
    float dx = fTar[0] - fPos[0];
    float dz = fTar[2] - fPos[2];
    // Yorozuya fix (non-IDA parity): SIMD-optimized 2D distance (x/z).
    return SseLength2D(dx, dz);
}

float GetDist(float *const a1, float *const a2)
{
    const float dx = *a2 - *a1;
    const float dz = a2[2] - a1[2];
    // Yorozuya fix (non-IDA parity): SIMD-optimized 2D distance (x/z).
    return SseLength2D(dx, dz);
}

float Get3DSqrt(float *Pos, float *Tar)
{
    const float dx = *Pos - *Tar;
    const float dy = Pos[1] - Tar[1];
    const float dz = Pos[2] - Tar[2];
    // Yorozuya fix (non-IDA parity): SIMD-optimized 3D distance (x/y/z).
    return SseLength3D(dx, dy, dz);
}

float GetYAngle(float *Pos, float *Tar)
{
    const float dx = *Tar - *Pos;
    const float dz = Tar[2] - Pos[2];
    // Yorozuya fix (non-IDA parity): SIMD-optimized 2D distance (x/z).
    const float dist = SseLength2D(dx, dz);
    if (dist == 0.0f)
    {
        return 0.0f;
    }

    const double angle = std::acos(dz / dist);
    if (dx <= 0.0f)
    {
        return static_cast<float>((-angle) * 65535.0 / 6.283185307 + 32768.0);
    }
    return static_cast<float>(angle * 65535.0 / 6.283185307 + 32768.0);
}

int GetQLen(float *fPos, float *fTar)
{
    const int dx = std::abs(static_cast<int>(fPos[0]) - static_cast<int>(fTar[0]));
    const int dz = std::abs(static_cast<int>(fPos[2]) - static_cast<int>(fTar[2]));
    if (dx <= dz)
    {
        return dz;
    }
    return dx;
}

float GetAngle(float *mon, float *plr)
{
    float dx = mon[0] - plr[0];
    float dz = mon[2] - plr[2];
    const float len = std::sqrt((dx * dx) + (dz * dz));
    if (len > 0.0f)
    {
        dx = dx / len;
        dz = dz / len;
    }

    float angle = std::acos(dx);
    if (!(dz > 0.0f))
    {
        angle = -angle;
    }
    return angle;
}

void Normalize(float *v)
{

    const float length = std::sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
    const double len = length;
    v[0] = v[0] / static_cast<float>(len);
    v[1] = v[1] / static_cast<float>(len);
    v[2] = v[2] / static_cast<float>(len);
}

void GetNormal(float *const outPlane, const float *const point1, const float *const point2, const float *const point3)
{
    const float edge1X = point2[0] - point1[0];
    const float edge1Y = point2[1] - point1[1];
    const float edge1Z = point2[2] - point1[2];
    const float edge2X = point3[0] - point1[0];
    const float edge2Y = point3[1] - point1[1];
    const float edge2Z = point3[2] - point1[2];

    outPlane[0] = edge1Y * edge2Z - edge1Z * edge2Y;
    outPlane[1] = edge1Z * edge2X - edge1X * edge2Z;
    outPlane[2] = edge1X * edge2Y - edge1Y * edge2X;

    const float magnitude = sqrtf(outPlane[0] * outPlane[0] + outPlane[1] * outPlane[1] + outPlane[2] * outPlane[2]);
    if (magnitude > 0.00001f)
    {
        outPlane[0] /= magnitude;
        outPlane[1] /= magnitude;
        outPlane[2] /= magnitude;
    }

    outPlane[3] = outPlane[0] * point1[0] + outPlane[1] * point1[1] + outPlane[2] * point1[2];
}

int GetPlaneCrossPoint(
    const float *const startPoint,
    const float *const endPoint,
    float *const crossPoint,
    const float *const planeNormal,
    float planeDistance)
{
    const float startDistance =
        (planeNormal[0] * startPoint[0] + planeNormal[1] * startPoint[1] + planeNormal[2] * startPoint[2]) - planeDistance;
    const float endDistance =
        (planeNormal[0] * endPoint[0] + planeNormal[1] * endPoint[1] + planeNormal[2] * endPoint[2]) - planeDistance;

    if ((startDistance < 0.0f && endDistance > 0.0f) || (startDistance >= 0.0f && endDistance < 0.0f))
    {
        const float blend = startDistance / (startDistance - endDistance);
        crossPoint[0] = (endPoint[0] - startPoint[0]) * blend + startPoint[0];
        crossPoint[1] = (endPoint[1] - startPoint[1]) * blend + startPoint[1];
        crossPoint[2] = (endPoint[2] - startPoint[2]) * blend + startPoint[2];
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

bool CheckEdge(const float *const a1, const float *const a2, const float *const a3, const float *const a4)
{
    float edge[4]{};
    edge[0] = a1[0] - a2[0];
    edge[1] = a1[1] - a2[1];
    edge[2] = a1[2] - a2[2];

    float cross[4]{};
    sub_1404E2FB0(const_cast<float *>(a4), edge, cross);

    const float dx = a3[0] - a2[0];
    const float dy = a3[1] - a2[1];
    const float dz = a3[2] - a2[2];
    return ((dx * cross[0]) + (dy * cross[1]) + (dz * cross[2])) <= 0.0f;
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

void sub_14050C650(float *destination, float *source, float *target)
{
    const float deltaX = source[0] - target[0];
    const float deltaY = source[1] - target[1];
    const float deltaZ = source[2] - target[2];
    const float distance = std::sqrt((deltaY * deltaY) + (deltaX * deltaX) + (deltaZ * deltaZ));
    destination[0] += (deltaX / distance) * 2.0f;
    destination[1] += (deltaY / distance) * 2.0f;
    destination[2] += (deltaZ / distance) * 2.0f;
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

float GetFloatMod(float value, float modulus)
{
    float modValue = value;
    if (modulus == 0.0f)
        return 0.0f;
    if (value < 0.0f)
        modValue = -value;
    while (modValue >= 32768.0f)
        modValue -= 32768.0f;
    return (static_cast<int>(modValue * 32768.0f) % static_cast<int>(modulus * 32768.0f)) * 0.000030517578f;
}

void MatrixIdentity(float (*const outMatrix)[4])
{
    std::memset(outMatrix, 0, 64);
    (*outMatrix)[0] = 1.0f;
    (*outMatrix)[5] = 1.0f;
    (*outMatrix)[10] = 1.0f;
    (*outMatrix)[15] = 1.0f;
}

void MatrixCopy(float (*const outMatrix)[4], float (*const sourceMatrix)[4])
{
    std::memcpy(outMatrix, sourceMatrix, 64);
}

void MatrixMultiply(float (*outMatrix)[4], float (*const leftMatrix)[4], float (*const rightMatrix)[4])
{
    float result[4][4]{};
    for (int row = 0; row < 4; ++row)
    {
        const int rowOffset = 4 * row;
        for (int column = 0; column < 4; ++column)
        {
            result[row][column] = ((*leftMatrix)[rowOffset] * (*rightMatrix)[column])
                                + ((*leftMatrix)[rowOffset + 1] * (*rightMatrix)[column + 4])
                                + ((*leftMatrix)[rowOffset + 2] * (*rightMatrix)[column + 8])
                                + ((*leftMatrix)[rowOffset + 3] * (*rightMatrix)[column + 12]);
        }
    }

    MatrixCopy(outMatrix, result);
}

int MatrixInvert(float (*const outMatrix)[4], float (*const inputMatrix)[4])
{
    if (std::fabs((*inputMatrix)[15] - 1.0f) > 0.001f)
        return 0;
    if (std::fabs((*inputMatrix)[3]) > 0.001f)
        return 0;
    if (std::fabs((*inputMatrix)[7]) > 0.001f)
        return 0;
    if (std::fabs((*inputMatrix)[11]) > 0.001f)
        return 0;

    const float m00 = (*inputMatrix)[0];
    const float m01 = (*inputMatrix)[1];
    const float m02 = (*inputMatrix)[2];
    const float m10 = (*inputMatrix)[4];
    const float m11 = (*inputMatrix)[5];
    const float m12 = (*inputMatrix)[6];
    const float m20 = (*inputMatrix)[8];
    const float m21 = (*inputMatrix)[9];
    const float m22 = (*inputMatrix)[10];
    const float tx = (*inputMatrix)[12];
    const float ty = (*inputMatrix)[13];
    const float tz = (*inputMatrix)[14];

    const float cofactor00 = m11 * m22 - m12 * m21;
    const float inverseDeterminant =
        1.0f / (cofactor00 * m00 - (m10 * m22 - m20 * m12) * m01 + (m10 * m21 - m20 * m11) * m02);
    const float inverse00 = cofactor00 * inverseDeterminant;
    const float inverse01 = -((m01 * m22 - m02 * m21) * inverseDeterminant);
    const float inverse02 = (m01 * m12 - m02 * m11) * inverseDeterminant;
    const float inverse10 = -((m22 * m10 - m12 * m20) * inverseDeterminant);
    const float inverse11 = (m22 * m00 - m02 * m20) * inverseDeterminant;
    const float inverse12 = -((m12 * m00 - m02 * m10) * inverseDeterminant);
    const float inverse20 = (m21 * m10 - m11 * m20) * inverseDeterminant;
    const float inverse21 = -((m00 * m21 - m01 * m20) * inverseDeterminant);
    const float inverse22 = (m00 * m11 - m01 * m10) * inverseDeterminant;

    (*outMatrix)[0] = inverse00;
    (*outMatrix)[1] = inverse01;
    (*outMatrix)[2] = inverse02;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[4] = inverse10;
    (*outMatrix)[5] = inverse11;
    (*outMatrix)[6] = inverse12;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[8] = inverse20;
    (*outMatrix)[9] = inverse21;
    (*outMatrix)[10] = inverse22;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[12] = -((inverse10 * ty) + (tx * inverse00) + (inverse20 * tz));
    (*outMatrix)[13] = -((tx * inverse01) + (ty * inverse11) + (inverse21 * tz));
    (*outMatrix)[14] = -((ty * inverse12) + (tx * inverse02) + (inverse22 * tz));
    (*outMatrix)[15] = 1.0f;
    return 1;
}

void MatrixFromQuaternion(float (*const outMatrix)[4], float x, float y, float z, float w)
{
    (*outMatrix)[15] = 1.0f;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[14] = 0.0f;
    (*outMatrix)[13] = 0.0f;
    (*outMatrix)[12] = 0.0f;
    (*outMatrix)[0] = 1.0f - (((z * z) + (y * y)) * 2.0f);
    (*outMatrix)[1] = ((x * y) - (z * w)) * 2.0f;
    (*outMatrix)[2] = ((y * w) + (x * z)) * 2.0f;
    (*outMatrix)[4] = ((z * w) + (x * y)) * 2.0f;
    (*outMatrix)[9] = ((x * w) + (y * z)) * 2.0f;
    (*outMatrix)[8] = ((x * z) - (y * w)) * 2.0f;
    (*outMatrix)[5] = 1.0f - (((z * z) + (x * x)) * 2.0f);
    (*outMatrix)[6] = ((y * z) - (x * w)) * 2.0f;
    (*outMatrix)[10] = 1.0f - (((y * y) + (x * x)) * 2.0f);
}

void MatrixScale(float (*const outMatrix)[4], float scaleX, float scaleY, float scaleZ)
{
    (*outMatrix)[0] = scaleX;
    (*outMatrix)[5] = scaleY;
    (*outMatrix)[15] = 1.0f;
    (*outMatrix)[1] = 0.0f;
    (*outMatrix)[2] = 0.0f;
    (*outMatrix)[10] = scaleZ;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[4] = 0.0f;
    (*outMatrix)[6] = 0.0f;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[8] = 0.0f;
    (*outMatrix)[9] = 0.0f;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[12] = 0.0f;
    (*outMatrix)[13] = 0.0f;
    (*outMatrix)[14] = 0.0f;
}

void MatrixRotate(float (*const outMatrix)[4], float rotationXDegrees, float rotationYDegrees, float rotationZDegrees)
{
    const double rotationYRadians = rotationYDegrees * 6.283184 / 360.0;
    const double rotationXRadians = rotationXDegrees * 6.283184 / 360.0;
    const double rotationZRadians = rotationZDegrees * 6.283184 / 360.0;

    float rotationYMatrix[4][4]{};
    const float rotationYCos = static_cast<float>(std::cos(rotationYRadians));
    const float rotationYSin = static_cast<float>(std::sin(rotationYRadians));
    rotationYMatrix[0][0] = rotationYCos;
    rotationYMatrix[0][2] = -rotationYSin;
    rotationYMatrix[1][1] = FLOAT_1_0;
    rotationYMatrix[2][0] = rotationYSin;
    rotationYMatrix[2][2] = rotationYCos;
    rotationYMatrix[3][3] = FLOAT_1_0;

    float rotationXMatrix[4][4]{};
    const float rotationXCos = static_cast<float>(std::cos(rotationXRadians));
    const float rotationXSin = static_cast<float>(std::sin(rotationXRadians));
    rotationXMatrix[0][0] = FLOAT_1_0;
    rotationXMatrix[1][1] = rotationXCos;
    rotationXMatrix[1][2] = rotationXSin;
    rotationXMatrix[2][1] = -rotationXSin;
    rotationXMatrix[2][2] = rotationXCos;
    rotationXMatrix[3][3] = FLOAT_1_0;

    float combinedRotationMatrix[4][4]{};
    MatrixMultiply(combinedRotationMatrix, rotationYMatrix, rotationXMatrix);

    float rotationZMatrix[4][4]{};
    const float rotationZCos = static_cast<float>(std::cos(rotationZRadians));
    const float rotationZSin = static_cast<float>(std::sin(rotationZRadians));
    rotationZMatrix[0][0] = rotationZCos;
    rotationZMatrix[0][1] = rotationZSin;
    rotationZMatrix[1][0] = -rotationZSin;
    rotationZMatrix[1][1] = rotationZCos;
    rotationZMatrix[2][2] = FLOAT_1_0;
    rotationZMatrix[3][3] = FLOAT_1_0;

    MatrixMultiply(outMatrix, rotationZMatrix, combinedRotationMatrix);
}

void MatrixRotateX(float (*const outMatrix)[4], float rotationDegrees)
{
    const double rotationRadians = rotationDegrees * 6.283184 / 360.0;
    const float rotationCos = static_cast<float>(std::cos(rotationRadians));
    const float rotationSin = static_cast<float>(std::sin(rotationRadians));

    (*outMatrix)[0] = 1.0f;
    (*outMatrix)[1] = 0.0f;
    (*outMatrix)[2] = 0.0f;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[4] = 0.0f;
    (*outMatrix)[5] = rotationCos;
    (*outMatrix)[10] = rotationCos;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[8] = 0.0f;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[12] = 0.0f;
    (*outMatrix)[13] = 0.0f;
    (*outMatrix)[6] = rotationSin;
    (*outMatrix)[14] = 0.0f;
    (*outMatrix)[15] = 1.0f;
    (*outMatrix)[9] = -rotationSin;
}

void MatrixRotateY(float (*const outMatrix)[4], float rotationDegrees)
{
    const double rotationRadians = rotationDegrees * 6.283184 / 360.0;
    const float rotationCos = static_cast<float>(std::cos(rotationRadians));
    const float rotationSin = static_cast<float>(std::sin(rotationRadians));

    (*outMatrix)[1] = 0.0f;
    (*outMatrix)[0] = rotationCos;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[4] = 0.0f;
    (*outMatrix)[5] = 1.0f;
    (*outMatrix)[10] = rotationCos;
    (*outMatrix)[6] = 0.0f;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[9] = 0.0f;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[12] = 0.0f;
    (*outMatrix)[13] = 0.0f;
    (*outMatrix)[8] = rotationSin;
    (*outMatrix)[14] = 0.0f;
    (*outMatrix)[15] = 1.0f;
    (*outMatrix)[2] = -rotationSin;
}

void MatrixRotateZ(float (*const outMatrix)[4], float rotationDegrees)
{
    const double rotationRadians = rotationDegrees * 6.283184 / 360.0;
    const float rotationCos = static_cast<float>(std::cos(rotationRadians));
    const float rotationSin = static_cast<float>(std::sin(rotationRadians));

    (*outMatrix)[0] = rotationCos;
    (*outMatrix)[5] = rotationCos;
    (*outMatrix)[10] = 1.0f;
    (*outMatrix)[2] = 0.0f;
    (*outMatrix)[3] = 0.0f;
    (*outMatrix)[6] = 0.0f;
    (*outMatrix)[7] = 0.0f;
    (*outMatrix)[8] = 0.0f;
    (*outMatrix)[1] = rotationSin;
    (*outMatrix)[9] = 0.0f;
    (*outMatrix)[11] = 0.0f;
    (*outMatrix)[12] = 0.0f;
    (*outMatrix)[13] = 0.0f;
    (*outMatrix)[14] = 0.0f;
    (*outMatrix)[4] = -rotationSin;
    (*outMatrix)[15] = 1.0f;
}

void QuaternionSlerp(
    float *outX,
    float *outY,
    float *outZ,
    float *outW,
    float startX,
    float startY,
    float startZ,
    float startW,
    float endX,
    float endY,
    float endZ,
    float endW,
    float blendFactor)
{
    float blendedStartX = startX;
    float blendedStartY = startY;
    float blendedStartZ = startZ;
    float blendedStartW = startW;
    float blendedEndX = endX;
    float blendedEndZ = endZ;
    float dotProduct = (((startX * endX) + (startY * endY)) + (startZ * endZ)) + (startW * endW);
    if (dotProduct < 0.0f)
    {
        blendedStartX = -startX;
        blendedStartY = -startY;
        blendedStartZ = -startZ;
        blendedStartW = -startW;
        dotProduct = -dotProduct;
    }

    float endWeight = 0.0f;
    float startWeight = 0.0f;
    if ((dotProduct + 1.0f) <= 0.050000001f)
    {
        endY = blendedStartX;
        endW = blendedStartZ;
        blendedEndX = -blendedStartY;
        blendedEndZ = -blendedStartW;
        startWeight = static_cast<float>(std::sin((0.5f - blendFactor) * 3.141592f));
        endWeight = static_cast<float>(std::sin(blendFactor * 3.141592f));
    }
    else if ((1.0f - dotProduct) >= 0.050000001f)
    {
        const float angle = std::acosf(dotProduct);
        const float sineAngle = std::sinf(angle);
        startWeight = std::sinf((1.0f - blendFactor) * angle) / sineAngle;
        endWeight = std::sinf(angle * blendFactor) / sineAngle;
    }
    else
    {
        endWeight = blendFactor;
        startWeight = 1.0f - blendFactor;
    }

    *outX = (startWeight * blendedStartX) + (endWeight * blendedEndX);
    *outY = (startWeight * blendedStartY) + (endWeight * endY);
    *outZ = (startWeight * blendedStartZ) + (endWeight * blendedEndZ);
    *outW = (startWeight * blendedStartW) + (endWeight * endW);
}

void GetMatrixFrom3DSMAXMatrix(float (*const matrix)[4])
{
    const float yx = (*matrix)[1];
    const float yy = (*matrix)[5];
    (*matrix)[1] = (*matrix)[2];
    const float yz = (*matrix)[6];
    (*matrix)[2] = yx;
    const float zy = (*matrix)[9];
    (*matrix)[6] = yy;
    (*matrix)[5] = yz;
    const float ty = (*matrix)[13];
    (*matrix)[9] = (*matrix)[10];
    const float tz = (*matrix)[14];
    (*matrix)[10] = zy;
    (*matrix)[14] = ty;
    (*matrix)[13] = tz;
}

static __m128 sub_14051F9D0(int keyframeCount, float *trackData, int trackStride, double animationTime)
{
    const int lastKeyframeIndex = keyframeCount - 1;
    if (lastKeyframeIndex > 0)
    {
        const float currentFrame = static_cast<float>(animationTime);
        for (int keyframeIndex = 0; keyframeIndex < lastKeyframeIndex; ++keyframeIndex)
        {
            const float startFrame = trackData[trackStride * keyframeIndex];
            const float endFrame = trackData[trackStride * (keyframeIndex + 1)];
            if (currentFrame >= startFrame && currentFrame < endFrame)
            {
                dword_184A893FC = keyframeIndex;
                dword_184A89400 = keyframeIndex + 1;
                return _mm_set_ps1((currentFrame - startFrame) / (endFrame - startFrame));
            }
        }
    }

    dword_184A893FC = lastKeyframeIndex;
    dword_184A89400 = lastKeyframeIndex;
    return _mm_setzero_ps();
}

void GetAniMatrix(float (*const outMatrix)[4], _ANI_OBJECT *animationObject, double animationTime)
{
    float interpolatedQuatX = 0.0f;
    float interpolatedQuatY = 0.0f;
    float interpolatedQuatZ = 0.0f;
    float interpolatedQuatW = 0.0f;
    float interpolatedScaleMatrix[4][4]{};
    float composedScaleMatrix[4][4]{};
    float scaleAxisMatrix[4][4]{};
    float inverseScaleAxisMatrix[4][4]{};

    MatrixIdentity(outMatrix);
    if (animationObject->Pos_cnt == 0 && animationObject->Rot_cnt == 0 && animationObject->Scale_cnt == 0)
    {
        MatrixFromQuaternion(
            outMatrix,
            animationObject->quat[0],
            animationObject->quat[1],
            animationObject->quat[2],
            animationObject->quat[3]);
        MatrixMultiply(outMatrix, outMatrix, animationObject->s_matrix);
        (*outMatrix)[12] = animationObject->pos[0];
        (*outMatrix)[13] = animationObject->pos[1];
        (*outMatrix)[14] = animationObject->pos[2];
        return;
    }

    if (animationObject->Rot_cnt)
    {
        const float rotationBlendFactor =
            _mm_cvtss_f32(sub_14051F9D0(animationObject->Rot_cnt, &animationObject->Rot->frame, 5, animationTime));
        const int currentRotationIndex = dword_184A893FC;
        const int nextRotationIndex = dword_184A89400;
        QuaternionSlerp(
            &interpolatedQuatX,
            &interpolatedQuatY,
            &interpolatedQuatZ,
            &interpolatedQuatW,
            animationObject->Rot[currentRotationIndex].quat[0],
            animationObject->Rot[currentRotationIndex].quat[1],
            animationObject->Rot[currentRotationIndex].quat[2],
            animationObject->Rot[currentRotationIndex].quat[3],
            animationObject->Rot[nextRotationIndex].quat[0],
            animationObject->Rot[nextRotationIndex].quat[1],
            animationObject->Rot[nextRotationIndex].quat[2],
            animationObject->Rot[nextRotationIndex].quat[3],
            rotationBlendFactor);
        MatrixFromQuaternion(outMatrix, interpolatedQuatX, interpolatedQuatY, interpolatedQuatZ, interpolatedQuatW);
    }
    else
    {
        MatrixFromQuaternion(
            outMatrix,
            animationObject->quat[0],
            animationObject->quat[1],
            animationObject->quat[2],
            animationObject->quat[3]);
    }

    if (animationObject->Scale_cnt)
    {
        const float scaleBlendFactor =
            _mm_cvtss_f32(sub_14051F9D0(animationObject->Scale_cnt, &animationObject->Scale->frame, 8, animationTime));
        MatrixIdentity(interpolatedScaleMatrix);

        _SCALE_TRACK *scaleTrack = animationObject->Scale;
        const int currentScaleIndex = dword_184A893FC;
        const int nextScaleIndex = dword_184A89400;
        interpolatedScaleMatrix[0][0] =
            ((scaleTrack[nextScaleIndex].scale[0] - scaleTrack[currentScaleIndex].scale[0]) * scaleBlendFactor)
            + scaleTrack[currentScaleIndex].scale[0];
        interpolatedScaleMatrix[1][1] =
            ((scaleTrack[nextScaleIndex].scale[1] - scaleTrack[currentScaleIndex].scale[1]) * scaleBlendFactor)
            + scaleTrack[currentScaleIndex].scale[1];
        interpolatedScaleMatrix[2][2] =
            ((scaleTrack[nextScaleIndex].scale[2] - scaleTrack[currentScaleIndex].scale[2]) * scaleBlendFactor)
            + scaleTrack[currentScaleIndex].scale[2];

        QuaternionSlerp(
            &interpolatedQuatX,
            &interpolatedQuatY,
            &interpolatedQuatZ,
            &interpolatedQuatW,
            scaleTrack[currentScaleIndex].scale_axis[0],
            scaleTrack[currentScaleIndex].scale_axis[1],
            scaleTrack[currentScaleIndex].scale_axis[2],
            scaleTrack[currentScaleIndex].scale_axis[3],
            scaleTrack[nextScaleIndex].scale_axis[0],
            scaleTrack[nextScaleIndex].scale_axis[1],
            scaleTrack[nextScaleIndex].scale_axis[2],
            scaleTrack[nextScaleIndex].scale_axis[3],
            scaleBlendFactor);
        MatrixFromQuaternion(scaleAxisMatrix, interpolatedQuatX, interpolatedQuatY, interpolatedQuatZ, interpolatedQuatW);
        MatrixInvert(inverseScaleAxisMatrix, scaleAxisMatrix);
        MatrixMultiply(composedScaleMatrix, interpolatedScaleMatrix, inverseScaleAxisMatrix);
        MatrixMultiply(composedScaleMatrix, scaleAxisMatrix, composedScaleMatrix);
        MatrixMultiply(outMatrix, outMatrix, composedScaleMatrix);
    }
    else
    {
        MatrixMultiply(outMatrix, outMatrix, animationObject->s_matrix);
    }

    if (!animationObject->Pos_cnt)
    {
        (*outMatrix)[12] = animationObject->pos[0];
        (*outMatrix)[13] = animationObject->pos[1];
        (*outMatrix)[14] = animationObject->pos[2];
        return;
    }

    const float positionBlendFactor =
        _mm_cvtss_f32(sub_14051F9D0(animationObject->Pos_cnt, &animationObject->Pos->frame, 4, animationTime));
    const int currentPositionIndex = dword_184A893FC;
    const int nextPositionIndex = dword_184A89400;
    (*outMatrix)[12] =
        ((animationObject->Pos[nextPositionIndex].pos[0] - animationObject->Pos[currentPositionIndex].pos[0])
         * positionBlendFactor)
        + animationObject->Pos[currentPositionIndex].pos[0];
    (*outMatrix)[13] =
        ((animationObject->Pos[nextPositionIndex].pos[1] - animationObject->Pos[currentPositionIndex].pos[1])
         * positionBlendFactor)
        + animationObject->Pos[currentPositionIndex].pos[1];
    (*outMatrix)[14] =
        ((animationObject->Pos[nextPositionIndex].pos[2] - animationObject->Pos[currentPositionIndex].pos[2])
         * positionBlendFactor)
        + animationObject->Pos[currentPositionIndex].pos[2];
}

static void sub_14051FF20(float (*outMatrix)[4], unsigned short objectIndex, _ANI_OBJECT *animationObjects, double animationTime)
{
    float localMatrix[4][4]{};
    float parentMatrix[4][4]{};
    if (objectIndex)
    {
        _ANI_OBJECT *animationObject = &animationObjects[objectIndex - 1];
        sub_14051FF20(parentMatrix, animationObject->parent, animationObjects, animationTime);
        GetAniMatrix(localMatrix, animationObject, animationTime);
        MatrixMultiply(outMatrix, parentMatrix, localMatrix);
    }
    else
    {
        MatrixIdentity(outMatrix);
    }
}

void GetObjectMatrix(float (*outMatrix)[4], unsigned short objectIndex, _ANI_OBJECT *animationObjects, double animationTime)
{
    if (!objectIndex)
    {
        MatrixIdentity(outMatrix);
        return;
    }

    _ANI_OBJECT *animationObject = &animationObjects[objectIndex - 1];
    if (animationObject->AniFrameCache)
    {
        MatrixCopy(outMatrix, animationObject->f_matrix);
        return;
    }

    const unsigned short parentIndex = animationObject->parent;
    animationObject->AniFrameCache = 1;

    double parentAnimationTime = 0.0;
    if (parentIndex)
    {
        const int parentFrameCount = animationObjects[parentIndex - 1].frames;
        if (parentFrameCount)
            parentAnimationTime = GetFloatMod(static_cast<float>(animationTime), static_cast<float>(parentFrameCount));
    }

    double objectAnimationTime = 0.0;
    if (animationObject->frames)
        objectAnimationTime = GetFloatMod(static_cast<float>(animationTime), static_cast<float>(animationObject->frames));

    float parentMatrix[4][4]{};
    float localMatrix[4][4]{};
    sub_14051FF20(parentMatrix, parentIndex, animationObjects, parentAnimationTime);
    GetAniMatrix(localMatrix, animationObject, objectAnimationTime);
    MatrixMultiply(outMatrix, parentMatrix, localMatrix);
    GetMatrixFrom3DSMAXMatrix(outMatrix);
    MatrixCopy(animationObject->f_matrix, outMatrix);
}

void ConvAniObject(int a1, unsigned __int8 *a2, _READ_ANI_OBJECT *a3, _ANI_OBJECT *a4)
{
    if (a1 <= 0)
        return;

    const _READ_ANI_OBJECT *srcObject = a3;
    _ANI_OBJECT *dstObject = a4;
    for (int objectIndex = 0; objectIndex < a1; ++objectIndex, ++srcObject, ++dstObject)
    {
        dstObject->flag = srcObject->flag;
        dstObject->parent = srcObject->parent;
        dstObject->frames = srcObject->frames;
        dstObject->Pos_cnt = srcObject->Pos_cnt;
        dstObject->Rot_cnt = srcObject->Rot_cnt;
        dstObject->Scale_cnt = srcObject->Scale_cnt;

        dstObject->scale[0] = srcObject->scale[0];
        dstObject->scale[1] = srcObject->scale[1];
        dstObject->scale[2] = srcObject->scale[2];

        dstObject->scale_quat[0] = srcObject->scale_quat[0];
        dstObject->scale_quat[1] = srcObject->scale_quat[1];
        dstObject->scale_quat[2] = srcObject->scale_quat[2];
        dstObject->scale_quat[3] = srcObject->scale_quat[3];

        dstObject->pos[0] = srcObject->pos[0];
        dstObject->pos[1] = srcObject->pos[1];
        dstObject->pos[2] = srcObject->pos[2];

        dstObject->quat[0] = srcObject->quat[0];
        dstObject->quat[1] = srcObject->quat[1];
        dstObject->quat[2] = srcObject->quat[2];
        dstObject->quat[3] = srcObject->quat[3];

        dstObject->Scale = reinterpret_cast<_SCALE_TRACK *>(a2 + srcObject->scale_offset);
        dstObject->Pos = reinterpret_cast<_POS_TRACK *>(a2 + srcObject->pos_offset);
        dstObject->Rot = reinterpret_cast<_ROT_TRACK *>(a2 + srcObject->rot_offset);
        dstObject->AniFrameCache = 0;

        _mm_prefetch(reinterpret_cast<const char *>(srcObject) + 560, 0);

        float scaleMatrix[4][4]{};
        float scaleQuatMatrix[4][4]{};
        float inverseScaleQuatMatrix[4][4]{};
        float composedScaleMatrix[4][4]{};

        MatrixIdentity(scaleMatrix);
        scaleMatrix[0][0] = srcObject->scale[0];
        scaleMatrix[1][1] = srcObject->scale[1];
        scaleMatrix[2][2] = srcObject->scale[2];

        MatrixFromQuaternion(
            scaleQuatMatrix,
            srcObject->scale_quat[0],
            srcObject->scale_quat[1],
            srcObject->scale_quat[2],
            srcObject->scale_quat[3]);
        MatrixInvert(inverseScaleQuatMatrix, scaleQuatMatrix);
        MatrixMultiply(composedScaleMatrix, scaleMatrix, inverseScaleQuatMatrix);
        MatrixMultiply(dstObject->s_matrix, scaleQuatMatrix, composedScaleMatrix);
    }
}
