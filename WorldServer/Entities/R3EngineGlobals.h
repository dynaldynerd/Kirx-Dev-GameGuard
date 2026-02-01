#pragma once

#include "IdaCompat.h"
#include "D3D8Compat.h"

struct CMergeFileManager;
struct _R3MATERIAL;
struct _LIGHTMAP;
struct R3Texture;

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
extern void *qword_184A79D68;
extern void *qword_184A79D70;
extern unsigned int dword_140978964;
extern unsigned int dword_140978968;
extern unsigned int dword_14097895C;
extern unsigned int dword_140978960;
extern unsigned int dword_184A79D60;
extern unsigned int dword_184A79D88;
extern unsigned int dword_184A79C54;
extern unsigned int dword_184A79DB4;
extern unsigned int stR3TexManageFlag;
extern unsigned int dword_184A797D0;
extern unsigned long long qword_184A79DA0;
extern _LIGHTMAP **stLightmap;
extern int *LightmapTexID;
extern unsigned long long qword_184A79C18;
extern unsigned long long qword_184A79C20;
extern unsigned long long qword_184A79818;
extern unsigned int dword_184A79920;
extern int dword_184A798D0;
extern unsigned int dword_184A79808;
extern unsigned int dword_184A79828;
extern unsigned int dword_184A7980C;
extern unsigned int dword_184A79810;
extern unsigned int dword_184A79A24;
extern unsigned int dword_184A79A28;
extern int dword_184A79824;
extern unsigned int dword_184A79814;
extern unsigned int dword_184A79844;
extern unsigned long long qword_184A79848;
extern unsigned int dword_184A7982C;
extern unsigned int dword_184A79834;
extern unsigned int dword_184A79778;
extern unsigned int dword_184A7981C;
extern unsigned int dword_184A7984C;
extern unsigned int dword_184A7983C;
extern unsigned int dword_184A79840;
extern unsigned int dword_184A79830;
extern unsigned int dword_184A79838;
extern unsigned int dword_184A79914;
extern unsigned int dword_184A79918;
extern unsigned int dword_184A7991C;
extern char byte_184A79850[128];
extern float unk_184A798D4[16];
extern int stRealSky;
extern int stShakeState;
extern int stDLightNum;

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
extern char aEbpAaiaiBsp[];
extern char aEbpAai[];
extern char aAaiai[];
extern char aAiAaiai_6[];
extern char aAiAaiaoA[];
extern char byte_1408851D8[];
extern char aAi_2[];
extern char aAo_0[];
extern char byte_140884F48[];
extern char asc_140884F60[];
extern char aAi_0[];
extern char aA_1[];
extern char aAi_1[];
extern char aA_3[];
extern char aA_0[];
extern char aA_2[];
extern char aAai[];
extern char aAi[];
extern char aAiR3tAai[];
extern char aAiAaiai_0[];
extern char aAiAaiai_1[];
extern char aAiAaiai_2[];
extern char aAiAaiai_3[];
extern char aAiAaiai_4[];
extern char aAiAaiai_5[];
extern char aAiAaiai[];
extern char aAiAaiaiAiau[];
extern char aAo_1[];
extern char aAo_2[];
extern char aR3tAaia[];
extern char aR3tAaia_0[];
extern char aAlphaSortTrue[];
extern char aTexture[];
extern char byte_140883FF8[];

// Environment globals.
extern char byte_184A790F0[];
extern char byte_184A79270[];
extern char byte_184A79924[256];
extern int unk_184A7999C[2];

// DDS unlock tables.
extern unsigned int pass_word[32];
extern unsigned int unk_1409788D4[32];
extern unsigned int unk_1409788D8[32];

// Shader usage flag (IDA global; mirrors R3 state).
extern int dword_184A797AC;

// Error/Warning handlers.
extern void (*ErrorMessageProc)(char *message);
extern void (*WarningMessageProc)(char *message);
extern int No_warning;
