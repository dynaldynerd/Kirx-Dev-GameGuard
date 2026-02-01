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
#include "R3EngineGlobals.h"
#include "CMergeFileManager.h"

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

void StripEXT(char *szPath)
{
  int idx = static_cast<int>(std::strlen(szPath)) - 1;
  for (int i = idx; i >= 0; --i)
  {
    if (szPath[i] == '.')
    {
      szPath[i] = '\0';
      return;
    }
  }
}

void StripPath(char *szPath)
{
  const int len = static_cast<int>(std::strlen(szPath));
  int slash = len - 2;
  for (int i = len - 2; i >= 0; --i)
  {
    if (szPath[i] == '\\')
      break;
    --slash;
  }
  const int start = slash + 1;
  if (start < len - 1)
  {
    const int count = len - 1 - start;
    char temp[256]{};
    memcpy_0(temp, &szPath[start], static_cast<unsigned int>(count));
    temp[count] = '\0';
    strcpy_s(szPath, 256, temp);
  }
}

bool IsServerMode()
{
    return dword_184A77F38 != 0;
}

bool IsInitR3Engine() { return dword_184A77F3C != 0; }
float R3GetTime() { return (float)GetTickCount() / 1000.0f; }

IDirect3DDevice8 *GetD3dDevice()
{
  return qword_184A7B2C8;
}

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
_R3MATERIAL *LoadMainMaterial(char *szFileName) { 
    (void)szFileName;
    return qword_184A79DA8;
}
_R3MATERIAL *GetMainMaterial() { return qword_184A79DA8; }
unsigned int GetMainMaterialNum() { return dword_184A79DB0; }
void SetMergeFileManager(CMergeFileManager *pMgr) { qword_184A7B208 = pMgr; }
void LoadLightMap(char *szFileName) {}
void CN_SetEnableSky(int bEnable) {}
void R3RestoreAllTextures() {}
void RTMovieCreate(char *szFileName, void *pLevel) {}
void RTMovieSetState(unsigned int nState) {}
void R3EnvironmentShakeOff() {}
void ClearDynamicLight() {}
void Error(char *source, char *msg)
{
  const char *v3 = source;
  char Text[256]{};

  char *dst = Text;
  while ((*dst++ = *source++) != '\0')
    ;

  char *v6 = &Text[std::strlen(Text) + 1];
  size_t idx = 0;
  while ((v6[idx++] = msg[idx - 1]) != '\0')
    ;

  FILE *v9 = nullptr;
  if (fopen_s(&v9, "error_message.txt", "wt") == 0 && v9)
  {
    fprintf(v9, "%s%s", v3, msg);
    fclose(v9);
  }

  void (*v10)(char *) = ErrorMessageProc;
  if (!v10)
  {
    MessageBoxA(nullptr, Text, "message", MB_ICONERROR);
    exit(1);
  }

  static const unsigned char asc_140884708[4] = { 0x3C, 0x2D, 0xBF, 0xA1 }; // "<-에"
  char *v11 = &Text[std::strlen(Text) + 1];
  memcpy_0(v11 - 1, asc_140884708, sizeof(asc_140884708));
  *reinterpret_cast<unsigned short *>(v11 + 3) = static_cast<unsigned short>(-20553);
  v11[5] = 0;
  v10(Text);
}

void Warning(char *source, char *msg)
{
  if (No_warning)
    return;

  char Text[256]{};
  char *dst = Text;
  while ((*dst++ = *source++) != '\0')
    ;
  char *v5 = &Text[std::strlen(Text) + 1];
  size_t idx = 0;
  while ((v5[idx++] = msg[idx - 1]) != '\0')
    ;

  void (*v8)(char *) = WarningMessageProc;
  if (v8)
  {
    static const char kSuffix[] = "\x3C\x2D\xBF\xA1\xB7\xAF"; // "<-에러"
    strcat_s(Text, kSuffix);
    v8(Text);
  }
  else
  {
    MessageBoxA(nullptr, Text, "message", 0);
  }
}

void ResetTotalVertexBufferInfo()
{
  dword_184A7B2D8 = 0;
  dword_184A7B2DC = 0;
}

unsigned int GetTotalVertexBufferSize()
{
  return dword_184A7B2D8;
}

unsigned int GetTotalVertexBufferCnt()
{
  return dword_184A7B2DC;
}

void *Dmalloc(int size)
{
  if (!size)
    return nullptr;

  dword_184A78FD4 += static_cast<unsigned int>(size);
  ++dword_184A78FD8;
  auto *v3 = static_cast<unsigned int *>(malloc(static_cast<size_t>(size) + 4));
  if (!v3)
    return nullptr;
  *v3 = static_cast<unsigned int>(size);
  return v3 + 1;
}

void Dfree(void *ptr)
{
  if (!ptr)
    return;

  auto *base = static_cast<unsigned int *>(ptr) - 1;
  unsigned int v1 = *base;
  --dword_184A78FD8;
  dword_184A78FD4 -= v1;
  free(base);
}

unsigned int GetDmallocSize()
{
  return dword_184A78FD4;
}

unsigned int GetDmallocCnt()
{
  return dword_184A78FD8;
}

bool IsParticle(char *a1)
{
  _strlwr(a1);
  const size_t len = std::strlen(a1);
  return len >= 3 && *reinterpret_cast<unsigned short *>(&a1[len - 2]) == 29808 && a1[len - 3] == 's';
}

CMergeFileManager *GetMergeFileManager()
{
  return qword_184A7B208;
}

FILE *fopenMFM(char *a1, char *Mode)
{
  char *v2 = Mode;
  char *v3 = a1;
  if (qword_184A7B208)
  {
    CMergeFileManager *mgr = GetMergeFileManager();
    FILE *result = mgr ? mgr->LoadFileOffset(v3, v2) : nullptr;
    if (result)
      return result;
    Mode = v2;
    a1 = v3;
  }
  return fopen(a1, Mode);
}

unsigned int GetFileSize(char *a1)
{
  FILE *v2 = fopen(a1, "rb");
  if (v2)
  {
    long v3 = ftell(v2);
    fseek(v2, 0, SEEK_END);
    long v4 = ftell(v2);
    fclose(v2);
    return static_cast<unsigned int>(v4 - v3);
  }
  return 0;
}

__int64 GetTokenFloat(char *a1, float *a2)
{
  int v4 = 0;
  int v5 = 0;
  __int64 v6 = 0;
  int v7 = static_cast<int>(std::strlen(a1));
  __int64 v8 = 0;
  char String[256]{};

  if (v7 <= 0)
  {
    *a2 = 0.0f;
    return static_cast<unsigned int>(v7);
  }

  while (true)
  {
    char v9 = a1[v6];
    if (v9 != '(')
      break;
    v5 = 0;
    v8 = 0;
LABEL_7:
    ++v6;
    ++v4;
    if (v6 >= v7)
    {
      *a2 = 0.0f;
      return static_cast<unsigned int>(v7);
    }
  }
  if (v9 != ',' && v9 != ')')
  {
    String[v8] = v9;
    ++v5;
    ++v8;
    goto LABEL_7;
  }
  String[v5] = 0;
  *a2 = static_cast<float>(std::atof(String));
  return static_cast<unsigned int>(v4 + 1);
}

__int64 GetRandOrNum(FILE *Stream, float *a2, float *a3)
{
  char String[256]{};
  char v17[256]{};

  fscanf(Stream, "%s", String);
  if (String[0] == 'r' && *reinterpret_cast<unsigned short *>(&String[1]) == 0x6E61 && String[3] == 'd')
  {
    int v6 = static_cast<int>(std::strlen(String));
    int v7 = 0;
    while (true)
    {
      if (v6 > 0)
      {
        for (__int64 v8 = 0; v8 < v6; ++v8)
        {
          if (String[v8] == ')')
            v7 = 1;
        }
        if (v7)
          break;
      }
      if (fscanf(Stream, "%s", v17) == -1 || !std::strcmp(String, "end"))
        Error(byte_140884F48, asc_140884F60);
      char *v9 = &String[std::strlen(String) + 1];
      __int64 v10 = 0;
      do
      {
        char v11 = v17[v10];
        v9[v10++ - 1] = v11;
      } while (v11);
      v6 = static_cast<int>(std::strlen(String));
    }
    int TokenFloat = static_cast<int>(GetTokenFloat(String, a2));
    GetTokenFloat(&String[TokenFloat], a3);
    return 1;
  }
  float v15 = static_cast<float>(std::atof(String));
  *a2 = v15;
  *a3 = v15;
  return 0;
}

__int64 IM_LoadWave(char *a1, unsigned int a2)
{
  (void)a1;
  (void)a2;
  return 1;
}

void IM_StopWave(unsigned int a1)
{
  (void)a1;
}

void IM_ReleaseWave(unsigned int a1)
{
  (void)a1;
}

void IM_ReleaseAllWaves()
{
}
