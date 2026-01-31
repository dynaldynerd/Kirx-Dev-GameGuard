#include "pch.h"

#include "WorldServerUtil.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>

CWnd *g_pFrame = nullptr;

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "CMerchant.h"
#include "GlobalObjectDefs.h"

unsigned int GetLoopTime()
{
  return timeGetTime();
}

CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax)
{
    unsigned int dwTime = timeGetTime();
    for (int i = 0; i < nMax; ++i)
    {
        if (!pNPC[i].m_bLive)
        {
            if (dwTime - pNPC[i].m_dwLastDestroyTime > 30000)
                return &pNPC[i];
        }
    }
    return nullptr;
}

void NetTrace(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    char Buffer[272]{};
    vsprintf_s(Buffer, sizeof(Buffer), fmt, va);
    va_end(va);
    strcat_s(Buffer, sizeof(Buffer), "\n");
    OutputDebugStringA(Buffer);
}

int GetCurrentDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

int GetCurDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

bool GetDateTimeStr(char *szTime)
{
  if (szTime == nullptr)
  {
    return false;
  }

  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return false;
  }

  return std::strftime(szTime, 0x80, "%y-%m-%d_%H-%M-%S", &local) != 0;
}

int GetItemTableCode(const char *psItemCode)
{
  if (psItemCode == nullptr)
  {
    return -1;
  }

  char prefix[3] = {};
  strncpy_s(prefix, sizeof(prefix), psItemCode, 2);

  if (strcmp(prefix, "iu") == 0) return 0;
  if (strcmp(prefix, "il") == 0) return 1;
  if (strcmp(prefix, "ig") == 0) return 2;
  if (strcmp(prefix, "is") == 0) return 3;
  if (strcmp(prefix, "ih") == 0) return 4;
  if (strcmp(prefix, "id") == 0) return 5;
  if (strcmp(prefix, "iw") == 0) return 6;
  if (strcmp(prefix, "im") == 0) return 11;
  if (strcmp(prefix, "ie") == 0) return 12;
  if (strcmp(prefix, "ip") == 0) return 13;
  if (strcmp(prefix, "ib") == 0) return 10;
  if (strcmp(prefix, "if") == 0) return 14;
  if (strcmp(prefix, "ic") == 0) return 15;
  if (strcmp(prefix, "it") == 0) return 16;
  if (strcmp(prefix, "io") == 0) return 17;
  if (strcmp(prefix, "ir") == 0) return 18;
  if (strcmp(prefix, "in") == 0) return 19;
  if (strcmp(prefix, "iy") == 0) return 20;
  if (strcmp(prefix, "ik") == 0) return 7;
  if (strcmp(prefix, "ii") == 0) return 8;
  if (strcmp(prefix, "ia") == 0) return 9;
  if (strcmp(prefix, "iz") == 0) return 21;
  if (strcmp(prefix, "iq") == 0) return 22;
  if (strcmp(prefix, "ix") == 0) return 23;
  if (strcmp(prefix, "ij") == 0) return 24;
  if (strcmp(prefix, "gt") == 0) return 25;
  if (strcmp(prefix, "tr") == 0) return 26;
  if (strcmp(prefix, "sk") == 0) return 27;
  if (strcmp(prefix, "ti") == 0) return 28;
  if (strcmp(prefix, "ev") == 0) return 29;
  if (strcmp(prefix, "re") == 0) return 30;
  if (strcmp(prefix, "bx") == 0) return 31;
  if (strcmp(prefix, "fi") == 0) return 32;
  if (strcmp(prefix, "un") == 0) return 33;
  if (strcmp(prefix, "rd") == 0) return 34;
  if (strcmp(prefix, "lk") == 0) return 35;
  if (strcmp(prefix, "cu") == 0) return 36;

  return -1;
}

int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize)
{
  char *src = pszSrc;
  for (int j = 0; j < nMaxWordNum; ++j)
  {
    char *dst = ppszDst[j];
    int len = 0;

    while (*src == ' ' || *src == '\t')
    {
      ++src;
    }

    while (*src != '\0' && *src != ' ' && *src != '\t')
    {
      *dst++ = *src++;
      if (len++ >= nMaxWordSize - 1)
      {
        return 0;
      }
    }
    *dst = '\0';

    if (*src == '\0')
    {
      if (std::strlen(ppszDst[j]) != 0)
      {
        return j + 1;
      }
      return j;
    }

    if (dst == ppszDst[j])
    {
      return j;
    }
    ++src;
  }

  return 0;
}

void WriteServerStartHistory(const char *format, ...)
{
  char buffer[512]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
}

int GetCurrentHour()
{
    return CTime::GetCurrentTime().GetHour();
}

int GetCurrentMin()
{
    return CTime::GetCurrentTime().GetMinute();
}

int GetCurrentSec()
{
    return CTime::GetCurrentTime().GetSecond();
}

void clear_file(const char *directory, int keepCount)
{
  (void)directory;
  (void)keepCount;
}

int MyMessageBox(const char *title, const char *format, ...)
{
  char buffer[1024]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  MessageBoxA(nullptr, buffer, title, MB_OK | MB_ICONERROR);
  return 0;
}

int MyCrtDebugReportHook(int reportType, char *message, int *returnValue)
{
  (void)reportType;
  (void)message;
  if (returnValue != nullptr)
  {
    *returnValue = 0;
  }
  return 0;
}

void ServerProgramExit(const char *source, int reason)
{
  char buffer[256]{};
  _snprintf_s(buffer, sizeof(buffer), _TRUNCATE, "ServerProgramExit: %s (%d)", source, reason);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  ExitProcess(static_cast<unsigned int>(reason));
}

char* StripEXT(char* szPath)
{
    char* pDot = strrchr(szPath, '.');
    if (pDot)
        *pDot = '\0';
    return szPath;
}

char* StripPath(char* szPath)
{
    char* pSlash = strrchr(szPath, '\\');
    if (pSlash)
        return pSlash + 1;
    return szPath;
}

bool IsServerMode()
{
    return true;
}

bool IsInitR3Engine() { return true; }
float R3GetTime() { return (float)GetTickCount() / 1000.0f; }

void ResetTexMemSize() {}
bool IsExistFile(char *szFileName) { 
    FILE* fp = nullptr; 
    if (fopen_s(&fp, szFileName, "rb") == 0 && fp) { fclose(fp); return true; } 
    return false;
}
void LoadMainR3M(char *szFileName) {}
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int a2) { return nullptr; }
void SetNoLodTextere() {}
void LoadR3T(struct R3Texture *pTex) {}
unsigned int GetNowTexMemSize() { return 0; }
void LoadMainMaterial(char *szFileName) {}
void SetMergeFileManager(void *pMgr) {}
void LoadLightMap(char *szFileName) {}
void CN_SetEnableSky(int bEnable) {}
void R3RestoreAllTextures() {}
void RTMovieCreate(char *szFileName, void *pLevel) {}
void RTMovieSetState(unsigned int nState) {}
void R3EnvironmentShakeOff() {}
void ClearDynamicLight() {}
void Error(char *source, char *msg) { MyMessageBox(source, msg); }

// Global helper stubs
void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3)
{
    a1[0] = (float)a2[0] + a3->pos[0];
    a1[1] = (float)a2[1] + a3->pos[1];
    a1[2] = (float)a2[2] + a3->pos[2];
}

void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3)
{
    a1[0] = (float)a2[0] * a3->scale + a3->pos[0];
    a1[1] = (float)a2[1] * a3->scale + a3->pos[1];
    a1[2] = (float)a2[2] * a3->scale + a3->pos[2];
}

void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3)
{
    // float vertex is absolute usually? or relative?
    // Decompiled code usually shows FVertex is relative too? Or absolute?
    // Checking CLevel... wait, this is used in CBsp. 
    // CBsp::OnlyStoreCollisionStructure uses it.
    // Let's assume absolute or relative same as others. Usually FVertex is relative.
    // Decompiled check: 
    // v22 = (float *)((char *)v + 12 * v21);
    // pos[0] = *v22 + mGroup.pos[0]; ...
    a1[0] = a2[0] + a3->pos[0];
    a1[1] = a2[1] + a3->pos[1];
    a1[2] = a2[2] + a3->pos[2];
}
