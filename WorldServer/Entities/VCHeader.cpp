#include "pch.h"

#include "VCHeader.h"

void VCHeader::set(unsigned __int16 encSize_, unsigned __int8 paddingSize_)
{
  encSize = encSize_;
  paddingSize = paddingSize_;
}
