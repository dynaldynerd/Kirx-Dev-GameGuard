#pragma once

#include "IdaCompat.h"
#include "D3D8Compat.h"

struct CMergeFileManager;
struct _R3MATERIAL;

// Memory tracking (Dmalloc/Dfree).
extern unsigned int dword_184A78FD4;
extern unsigned int dword_184A78FD8;

// Vertex buffer tracking.
extern unsigned int dword_184A7B2D8;
extern unsigned int dword_184A7B2DC;

// Engine/server mode flags.
extern unsigned int dword_184A77F38;
extern unsigned int dword_184A77F3C;

// D3D device + merge file manager pointers.
extern IDirect3DDevice8 *qword_184A7B2C8;
extern CMergeFileManager *qword_184A7B208;

// Main material globals.
extern _R3MATERIAL *qword_184A79DA8;
extern unsigned int dword_184A79DB0;
extern unsigned long long qword_184A79818;
extern unsigned int dword_184A79920;
extern int dword_184A798D0;
extern unsigned int dword_184A79808;
extern unsigned int dword_184A79828;
extern unsigned int dword_184A7980C;
extern unsigned int dword_184A79810;
extern unsigned int dword_184A79A24;
extern unsigned int dword_184A79A28;

// Animation interpolation indices.
extern int dword_184A893FC;
extern int dword_184A89400;

// Shared static strings (cp949 bytes where applicable).
extern char byte_140883769[];
extern char byte_140884910[];
extern char byte_140884930[];
extern char aAiu[];
extern char aAiao[];
extern char aMaxVbCnt[];
extern char aMakeedgenormal[];
extern char aR3engineAe[];
extern char aAaiai_0[];
extern char aBspAai[];
extern char aAa[];
extern char aRpkAaiAo[];
extern char asc_140884778[];

// Environment globals.
extern char byte_184A790F0[];
extern char byte_184A79924[256];
extern int unk_184A7999C[2];

// Error/Warning handlers.
extern void (*ErrorMessageProc)(char *message);
extern void (*WarningMessageProc)(char *message);
extern int No_warning;
