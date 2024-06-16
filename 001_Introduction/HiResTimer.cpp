#include "HiResTimer.h"


HiResTimer::HiResTimer()
{
   LARGE_INTEGER frequency;
   QueryPerformanceFrequency(&frequency);
   FrequencyInverse = 1. / static_cast<double>(frequency.QuadPart);
   QueryPerformanceCounter(&StartTicks);
}


double HiResTimer::GetElapsed() const
{
   LARGE_INTEGER currentTicks;
   QueryPerformanceCounter(&currentTicks);
   return static_cast<double>(currentTicks.QuadPart - StartTicks.QuadPart) * FrequencyInverse;
}
