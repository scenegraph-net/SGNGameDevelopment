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

      int PlayerScore;

      void UpdateGameState(double frameTime);
      void DrawGameEntities(HDC surfaceContext);
      void DrawPlayerScore(HDC surfaceContext);

      void CheckForWallCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;
      void CheckForBrickCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition);
      void CheckForPaddleCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;

      void HandleImpact(const Collision& impact);

      void HandleRectangleCollision(const Box2D& extent, CollisionSide side);
      void HandleWallCollision(CollisionSide side);

      void PointBounce(const glm::vec2& point);

      void BrickImpact(Brick& brick);

      static bool DetectBallVsRectangleCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition,
         const Box2D& rectangleExtent, CollisionSide& out_collisionSide, double& out_collisionTime);

      static bool DetectBallVsPointCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition,
         const glm::vec2& testPoint, double& out_collisionTime);
};
