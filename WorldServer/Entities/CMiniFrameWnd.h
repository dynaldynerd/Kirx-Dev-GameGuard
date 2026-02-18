#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CWnd
{
};

class CFrameWnd : public CWnd
{
};

class  CMiniFrameWnd : public CFrameWnd
{
};
#endif
