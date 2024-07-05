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

      std::vector<Brick> Bricks;
      Paddle PlayerPaddle;
      Ball PlayerBall;

      void UpdateGameState(double frameTime);
      void DrawGameEntities(HDC surfaceContext);
};
