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

      void Update(double frameTime);

   private:
      HWND WindowHandle;
      POINT MousePosition;
      DrawingSurface Surface;

      POINTFLOAT BallPosition;
      POINTFLOAT BallVelocity;

      std::vector<Brick> Bricks;
};
