#include "pch.h"

#include "R3TimerCompat.h"

#include <windows.h>
#include <mmsystem.h>

#include "R3EngineGlobals.h"

namespace
{
bool s_timerInitialized = false;
unsigned __int64 s_ticksPerSecond = 0;
float s_secondsPerFrame = 0.001f;
float s_ticksPerSecondFloat = 0.0f;
unsigned __int64 s_ticksPerFrame = 0;
unsigned __int64 s_ticks = 0;
unsigned __int64 s_startTicks = 0;
int s_loopCount = 0;
unsigned int s_oldTime = 0;
unsigned int s_loopHop = 0;
float s_timerRate = 1.0f;
float s_minLoopSeconds = 0.001f;

float s_blinkAccum30 = 0.0f;
float s_blinkAccum15 = 0.0f;
float s_blinkAccum7 = 0.0f;
int s_blinkState30 = 0;
int s_blinkState15 = 0;
int s_blinkState7 = 0;

float ReadLoopSeconds()
{
  float value = 0.0f;
  static_assert(sizeof(value) == sizeof(dword_184A77EF0), "Loop seconds storage size mismatch");
  memcpy_0(&value, &dword_184A77EF0, sizeof(value));
  return value;
}

void WriteLoopSeconds(float value)
{
  static_assert(sizeof(value) == sizeof(dword_184A77EF0), "Loop seconds storage size mismatch");
  memcpy_0(&dword_184A77EF0, &value, sizeof(value));
}

float ReadAccumulatedSeconds()
{
  float value = 0.0f;
  static_assert(sizeof(value) == sizeof(dword_184A77EE8), "Accumulated seconds storage size mismatch");
  memcpy_0(&value, &dword_184A77EE8, sizeof(value));
  return value;
}

void WriteAccumulatedSeconds(float value)
{
  static_assert(sizeof(value) == sizeof(dword_184A77EE8), "Accumulated seconds storage size mismatch");
  memcpy_0(&dword_184A77EE8, &value, sizeof(value));
}

LARGE_INTEGER QueryHighResCounter()
{
  LARGE_INTEGER counter{};
  if (QueryPerformanceCounter(&counter))
  {
    while (!counter.QuadPart)
    {
      QueryPerformanceCounter(&counter);
    }
    return counter;
  }

  LARGE_INTEGER fallback{};
  fallback.QuadPart = timeGetTime();
  return fallback;
}

void UpdateBlinkAccumulator(float &accumulator, int &state, float threshold)
{
  const float next = accumulator + ReadLoopSeconds();
  accumulator = next;
  if (accumulator >= threshold)
  {
    state = -state;
    accumulator = next - threshold;
    if (accumulator >= threshold)
    {
      accumulator = 0.0f;
    }
  }
}
} // namespace

void R3CalculateTime()
{
  if (!s_timerInitialized)
  {
    s_timerInitialized = true;
    LARGE_INTEGER frequency{};
    if (!QueryPerformanceFrequency(&frequency))
    {
      frequency.QuadPart = 1000;
    }

    s_ticksPerSecond = static_cast<unsigned __int64>(frequency.QuadPart);
    s_secondsPerFrame = 0.001f;
    s_ticksPerSecondFloat = static_cast<float>(static_cast<int>(s_ticksPerSecond));
    s_ticksPerFrame = s_ticksPerSecond / 1000;
    s_ticks = static_cast<unsigned __int64>(QueryHighResCounter().QuadPart);
    s_startTicks = s_ticks;
  }

  const unsigned int currentTime = timeGetTime();
  const int loopCount = ++s_loopCount;
  const unsigned int elapsed = currentTime - s_oldTime + s_loopHop;
  s_loopHop = elapsed;

  float loopSeconds = ReadLoopSeconds();
  if (elapsed > 10)
  {
    s_loopCount = 0;
    s_loopHop = 0;
    loopSeconds = (static_cast<float>(static_cast<int>(elapsed)) / static_cast<float>(loopCount)) / 1000.0f;
  }

  s_oldTime = currentTime;
  if (loopSeconds > s_minLoopSeconds || loopSeconds < 0.0f)
  {
    loopSeconds = s_minLoopSeconds;
  }

  const float adjustedLoopSeconds = loopSeconds * s_timerRate;
  WriteLoopSeconds(adjustedLoopSeconds);
  WriteAccumulatedSeconds(ReadAccumulatedSeconds() + adjustedLoopSeconds);

  UpdateBlinkAccumulator(s_blinkAccum30, s_blinkState30, 0.033333335f);
  UpdateBlinkAccumulator(s_blinkAccum15, s_blinkState15, 0.06666667f);
  UpdateBlinkAccumulator(s_blinkAccum7, s_blinkState7, 0.14285715f);
}
