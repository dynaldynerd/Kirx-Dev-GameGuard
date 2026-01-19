#pragma once

#include "IdaCompat.h"

#ifndef _MFC_VER
class CWnd
{
};

class CDialog : public CWnd
{
};

class __cppobj CMultiPageDHtmlDialog : public CDialog
{
};
#endif
