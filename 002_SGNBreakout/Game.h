#pragma once

#include <windows.h>

#include "DrawingSurface.h"


class Game
{
   public:
      Game(HWND windowHandle);

      void SetMousePosition(unsigned short x, unsigned short y);

      void Update();

   private:
      HWND WindowHandle;
      POINT MousePosition;
      DrawingSurface Surface;
};
