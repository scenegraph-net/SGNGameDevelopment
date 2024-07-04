#pragma once

#include <windows.h>


class HiResTimer
{
   public:
      HiResTimer();
      double GetElapsed() const;

   private:
      LARGE_INTEGER StartTicks;
      double FrequencyInverse;
};
