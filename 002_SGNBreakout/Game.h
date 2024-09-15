#pragma once

#include <memory>
#include <vector>

#include <windows.h>

#include "DrawingSurface.h"
#include "GameEntities.h"
#include "Collision.h"
#include "BitmapImage.h"
#include "DrawManager.h"


enum class AppState
{
   INIT,
   READY,
   PLAYING,
   LEVEL_COMPLETED,
   LOST,
};


struct BrickEntry;


class Game
{
   public:
      Game(HWND windowHandle);

      void SetMousePosition(int x, int y);
      void MouseClick();

      void Update(double frameTime);

   private:
      HWND WindowHandle;
      POINT MousePosition;
      DrawingSurface Surface;

      DrawManager MainWindowDrawManager;

      Paddle PlayerPaddle;
      Ball PlayerBall;

      AppState CurrentAppState;
      std::unique_ptr<Level> CurrentLevel;
      int CurrentLevelIndex;

      int PlayerScore;
      int PlayerLives;

      std::vector<BrickType> BrickTypes;

      void SetupBrickTypes();

      void LoadLevel();
      void ParseLevelData(const std::vector<unsigned char>& levelData, std::vector<BrickEntry>& brickEntries);
      void CreateLevel(const std::vector<BrickEntry>& brickEntries);

      void ResetBall();
      void ResetScore();

      void UpdateGameState(double frameTime);
      void DrawGameEntities(HDC surfaceContext);
      void DrawPlayerScore(HDC surfaceContext);
      void DrawReadyMessage(HDC surfaceContext);
      void DrawGameOverMessage(HDC surfaceContext);
      void DrawLevelCompletedMessage(HDC surfaceContext);

      void CheckForWallCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;
      void CheckForBrickCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition);
      void CheckForPaddleCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const;

      void HandleImpact(const Collision& impact);

      void HandleRectangleCollision(const Box2D& extent, CollisionSide side);
      void HandleWallCollision(CollisionSide side);

      void PointBounce(const glm::vec2& point);

      void BrickImpact(Brick& brick);

      void ChangeAppState(AppState newAppState);

      static bool DetectBallVsRectangleCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition,
         const Box2D& rectangleExtent, CollisionSide& out_collisionSide, double& out_collisionTime);

      static bool DetectBallVsPointCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition,
         const glm::vec2& testPoint, double& out_collisionTime);
};
