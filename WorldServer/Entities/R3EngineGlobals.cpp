#include "pch.h"

#include "R3EngineGlobals.h"

// Memory tracking.
unsigned int dword_184A78FD4 = 0;
unsigned int dword_184A78FD8 = 0;

// Vertex buffer tracking.
unsigned int dword_184A7B2D8 = 0;
unsigned int dword_184A7B2DC = 0;

// Engine/server mode flags.
unsigned int dword_184A77F38 = 1; // server mode by default
unsigned int dword_184A77F3C = 1; // R3 engine initialized by default

// D3D device + merge file manager pointers.
IDirect3DDevice8 *qword_184A7B2C8 = nullptr;
CMergeFileManager *qword_184A7B208 = nullptr;

// Main material globals.
_R3MATERIAL *qword_184A79DA8 = nullptr;
unsigned int dword_184A79DB0 = 0;
unsigned long long qword_184A79818 = 0;
unsigned int dword_184A79920 = 0;
int dword_184A798D0 = 0;
unsigned int dword_184A79808 = 0;
unsigned int dword_184A79828 = 0;
unsigned int dword_184A7980C = 0;
unsigned int dword_184A79810 = 0;
unsigned int dword_184A79A24 = 0;
unsigned int dword_184A79A28 = 0;

int dword_184A893FC = 0;
int dword_184A89400 = 0;

// Shared static strings (cp949 bytes where applicable).
char byte_140883769[] = "";
char byte_140884910[] = "\xB9\xF6\xC5\xD8\xBD\xBA\x20\xB9\xF6\xC6\xDB\xB8\xA6\x20\xBB\xFD\xBC\xBA\xC7\xD2\xBC\xF6\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E"; // "버텍스 버퍼를 생성할수없습니다."
char byte_140884930[] = "\xB9\xF6\xC5\xD8\xBD\xBA\xB9\xF6\xC6\xDB\xB8\xA6\x20\x6C\x6F\x63\x6B\x20\xC7\xD2\xBC\xF6\xB0\xA1\x20\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E"; // "버텍스버퍼를 lock 할수가 없습니다."
char aAiu[] = "\xC0\xCE\xB5\xA6\xBD\xBA\x20\xB9\xF6\xC6\xDB\xB8\xA6\x20\xBB\xFD\xBC\xBA\xC7\xD2\xBC\xF6\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E"; // "인덱스 버퍼를 생성할수없습니다."
char aAiao[] = "\xC0\xCE\xC5\xD8\xBD\xBA\xB9\xF6\xC6\xDB\xB8\xA6\x20\x6C\x6F\x63\x6B\x20\xC7\xD2\xBC\xF6\xB0\xA1\x20\xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9\x2E"; // "인덱스버퍼를 lock 할수가 없습니다."
char aMaxVbCnt[] = "\x5F\x4D\x41\x58\x5F\x56\x42\x5F\x43\x4E\x54\xB8\xA6\x20\xB3\xD1\xB0\xE5\xBD\xC0\xB4\xCF\xB4\xD9\x2E"; // "_MAX_VB_CNT를 넘겼습니다."
char aMakeedgenormal[] =
  "\x4D\x61\x6B\x65\x45\x64\x67\x65\x4E\x6F\x72\x6D\x61\x6C\x28\x29\xB4\xC2\x20\xBC\xAD\xB9\xF6\xB8\xF0\xB5\xE5\x20\xC0\xCF\xB6\xA7\xB8\xB8\x20\xBD\xE1\xBE\xDF\xB5\xCA\x2E"; // "MakeEdgeNormal()는 서버모드 일때만 써야됨."
char aR3engineAe[] =
  "\x52\x33\x65\x6E\x67\x69\x6E\x65\x20\xC3\xCA\xB1\xE2\xC8\xAD\x20\xBA\xCE\xC5\xCD\xC7\xCF\xBC\xBC\xBF\xE4\x2E"; // "R3engine 초기화 부터하세요."
char aAaiai_0[] = "\x3C\x2D\x20\xC6\xC4\xC0\xCF\xC0\xCC\x20\xBE\xF8\xBD\xBF\xB4\xD9\x2E"; // "<- 파일이 없슴다."
char aBspAai[] = "\x42\x53\x50\xC6\xC4\xC0\xCF\x20\xB9\xF6\xC1\xAF\xC0\xCC\x20\xB8\xC2\xC1\xF6\xBE\xCA\xBD\xBF\xB4\xD9\x2E"; // "BSP파일 버젼이 맞지않슴다."
char aAa[] = "\xC0\xE5\xBE\xF0\xC0\xCF"; // "장언일"
char aRpkAaiAo[] = ""; // value not recovered yet
char asc_140884778[] = ""; // value not recovered yet

// Environment globals.
char byte_184A790F0[] = ".\\Map\\Entity\\";
char byte_184A79924[256] = {};
int unk_184A7999C[2] = {};

// Error/Warning handlers.
void (*ErrorMessageProc)(char *message) = nullptr;
void (*WarningMessageProc)(char *message) = nullptr;
int No_warning = 0;
