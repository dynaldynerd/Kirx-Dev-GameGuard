#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CWnd
{
};

class CFrameWnd : public CWnd
{
};

class __cppobj CMiniFrameWnd : public CFrameWnd
{
};
#endif
