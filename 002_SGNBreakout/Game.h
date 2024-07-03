#pragma once

#include <vector>

#include <windows.h>

#include "DrawingSurface.h"
#include "GameEntities.h"


class Game
{
   public:
      Game(HWND windowHandle);

      void SetMousePosition(int x, int y);

      void Update();

   private:
      HWND WindowHandle;
      POINT MousePosition;
      DrawingSurface Surface;

      std::vector<Brick> Bricks;
};
