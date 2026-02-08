#include "pch.h"

#include "USThread.h"

namespace US
{
  CNoneCopyAble::CNoneCopyAble() = default;
  CNoneCopyAble::~CNoneCopyAble() = default;

  AbstractThread::AbstractThread() = default;
  AbstractThread::~AbstractThread() = default;

  AbstractThreadPool::~AbstractThreadPool() = default;
} // namespace US
