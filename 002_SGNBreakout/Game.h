#pragma once

#include <vector>

#include <windows.h>

#include "DrawingSurface.h"
#include "GameEntities.h"
#include "Collision.h"


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

      void CheckForWallCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;
      void CheckForBrickCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;
      void CheckForPaddleCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;

      void HandleImpact(const Collision& impact);
};
