#pragma once

#include "IdaCompat.h"

class CWnd;

unsigned int GetLoopTime();
void WriteServerStartHistory(const char *format, ...);
void clear_file(const char *directory, int keepCount);
int MyMessageBox(const char *title, const char *format, ...);
int MyCrtDebugReportHook(int reportType, char *message, int *returnValue);

extern CWnd *g_pFrame;
