#pragma once

#include <windows.h>

#include "DrawingSurface.h"


class Game
{
   public:
      Game(HWND windowHandle);

      void SetMousePosition(unsigned short x, unsigned short y);

      void ResizeWindow(unsigned short width, unsigned short height);
      void Update();

   private:
      HWND WindowHandle;
      POINT MousePosition;
      DrawingSurface Surface;
};
