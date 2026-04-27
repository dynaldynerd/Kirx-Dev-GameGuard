#ifndef _COMMONUTIL_H_
#define _COMMONUTIL_H_

#include "windows.h"

void StripEXT(char *name);
void StripPath(char *name);
void StripName(char *name);
BOOL IsExistFile(char *name);
BOOL GetNowPath(char *get_path);
void GetFinalPath(char *get,char *path);
void StripFinalPath(char *name);
void GetEXT(char *get,char *name);

int GetFileSize(char *name);

int MaxFixFloatToInt(float su);
int MinFixFloatToInt(float su);

void R3CalculateTime();
float R3GetLoopTime();
void R3SetMinFPS(float min_fps);
void R3SetTime(float time);
float R3GetTime();
void R3SetLoopTime(float time);

LONG R3GetToggle30fps();
LONG R3GetToggle15fps();
LONG R3GetToggle7fps();

float GetFPS();
void SaveBMP(char *name,int xl,int yl,int pitch,BYTE *argb);
void SaveJPG(char *name,int xl,int yl,int pitch,BYTE *argb);

#endif
