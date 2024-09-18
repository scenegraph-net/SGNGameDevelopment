#include "Game.h"

#include <algorithm>
#include <array>
#include <format>
#include <sstream>

#include "DrawManager.h"
#include "MathUtils.h"

#include "ResourceDataLoader.h"

#include "resource.h"


static constexpr int WINDOW_LEFT_MARGIN = 24;
static constexpr int WINDOW_TOP_MARGIN = 24;

static constexpr int PLAYING_FIELD_WIDTH = 720;
static constexpr int PLAYING_FIELD_HEIGHT = 528;

static constexpr int INITIAL_PLAYER_LIVES = 3;

static constexpr int BRICK_WIDTH = 48;
static constexpr int BRICK_HEIGHT = 24;

static constexpr RECT BALL_AREA =
{
   static_cast<LONG>(WINDOW_LEFT_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_LEFT_MARGIN + PLAYING_FIELD_WIDTH - Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT - Ball::RADIUS)
};


struct BrickEntry
{
   glm::ivec2 Position;
   unsigned short TypeIndex;
};


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ }), CurrentAppState(AppState::Init), CurrentLevelIndex(0),
   MainWindowDrawManager(IDB_BITMAP_BACKGROUND, IDB_BITMAP_PADDLE)
{
   SetupBrickTypes();

   ResetBall();
   ResetScore();

   LoadLevel();

   PlayerPaddle.PositionX = 0;
   PlayerPaddle.Extent.GetUpperLeft().y = static_cast<float>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT) - Paddle::HEIGHT;
   PlayerPaddle.Extent.GetLowerRight().y = static_cast<float>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT);

   ChangeAppState(AppState::Ready);
}


void Game::SetupBrickTypes()
{
   std::vector<int> brickImageResourceIds = {
      IDB_BITMAP_BRICK_0,
      IDB_BITMAP_BRICK_1,
      IDB_BITMAP_BRICK_2,
      IDB_BITMAP_BRICK_3,
      IDB_BITMAP_BRICK_4,
      IDB_BITMAP_BRICK_5,
      IDB_BITMAP_BRICK_6,
   };

   for (const int resourceId : brickImageResourceIds)
      BrickTypes.emplace_back(MainWindowDrawManager.AddBrickType(resourceId));
}


void Game::ResetBall()
{
   PlayerBall.Position = { 200.f, 400.f };
   PlayerBall.Velocity = { 150.f, 150.f };
}


void Game::LoadLevel()
{
   std::array<int, 2> levelResourceIds{ IDR_LEVEL_0001, IDR_LEVEL_0002 };
   const int levelIndex = CurrentLevelIndex % levelResourceIds.size();

   std::vector<unsigned char> levelData;
   std::unique_ptr<DataLoader> dataLoader = std::make_unique<ResourceDataLoader>("LEVEL", levelResourceIds[levelIndex]);
   dataLoader->Load(levelData);

   std::vector<BrickEntry> brickEntries;
   ParseLevelData(levelData, brickEntries);

   CreateLevel(brickEntries);
}


void Game::ParseLevelData(const std::vector<unsigned char>& levelData, std::vector<BrickEntry>& brickEntries)
{
   std::string levelText(levelData.begin(), levelData.end());

   std::istringstream dataStream(levelText);
   std::string line;

   while (std::getline(dataStream, line))
   {
      if (line.length() < 5 || line[0] == '#')
         continue;

      unsigned short brickX;
      unsigned short brickY;
      unsigned short brickTypeIndex;

      std::istringstream lineStream(line);

      lineStream >> brickX;
      lineStream >> brickY;
      lineStream >> brickTypeIndex;

      brickEntries.push_back(BrickEntry{ { brickX, brickY }, brickTypeIndex });
   }

   return;
}


void Game::CreateLevel(const std::vector<BrickEntry>& brickEntries)
{
   CurrentLevel = std::make_unique<Level>();

   for (const auto& brickEntry : brickEntries)
   {
      const float xmin = static_cast<float>(brickEntry.Position.x * BRICK_WIDTH + WINDOW_LEFT_MARGIN);
      const float xmax = static_cast<float>(brickEntry.Position.x * BRICK_WIDTH + WINDOW_LEFT_MARGIN + BRICK_WIDTH);

      const float ymin = static_cast<float>(brickEntry.Position.y * BRICK_HEIGHT + WINDOW_TOP_MARGIN);
      const float ymax = static_cast<float>(brickEntry.Position.y * BRICK_HEIGHT + WINDOW_TOP_MARGIN + BRICK_HEIGHT);

      const Box2D brickExtent(glm::vec2(xmin, ymin), glm::vec2(xmax, ymax));
      unsigned short typeIndex = std::min(brickEntry.TypeIndex, static_cast<unsigned short>(BrickTypes.size()));
      CurrentLevel->Bricks.emplace_back(brickExtent, BrickTypes[typeIndex]);
   }

   CurrentLevel->ActiveBrickCount = CurrentLevel->Bricks.size();
}


void Game::ResetScore()
{
   PlayerScore = 0;
   PlayerLives = INITIAL_PLAYER_LIVES;
}


void Game::SetMousePosition(int x, int y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void Game::MouseClick()
{
   switch (CurrentAppState)
   {
      case AppState::Ready:
         ChangeAppState(AppState::Playing);
         break;
      case AppState::Lost:
         ResetScore();
         LoadLevel();
         ChangeAppState(AppState::Ready);
         break;
      case AppState::LevelCompleted:
         CurrentLevelIndex++;
         LoadLevel();
         ChangeAppState(AppState::Ready);
         break;
   }
}


void Game::Update(double frameTime)
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   HDC surfaceContext = Surface.GetDeviceContext();
   MainWindowDrawManager.DrawBackground(surfaceContext, clientRectangle);

   switch (CurrentAppState)
   {
      case AppState::Playing:
         UpdateGameState(frameTime);
         DrawGameEntities(surfaceContext);
         DrawPlayerScore(surfaceContext);
         break;
      case AppState::Ready:
         UpdateGameState(frameTime);
         DrawGameEntities(surfaceContext);
         DrawPlayerScore(surfaceContext);
         DrawReadyMessage(surfaceContext);
         break;
      case AppState::LevelCompleted:
         DrawLevelCompletedMessage(surfaceContext);
         break;
      case AppState::Lost:
         DrawGameEntities(surfaceContext);
         DrawGameOverMessage(surfaceContext);
         break;
   }

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}


void Game::UpdateGameState(double frameTime)
{
   const float paddlePosition = std::min(
      std::max(.0f, static_cast<float>(MousePosition.x)), 
      static_cast<float>(PLAYING_FIELD_WIDTH) - Paddle::WIDTH
   );

   PlayerPaddle.Extent.GetUpperLeft().x = WINDOW_LEFT_MARGIN + paddlePosition;
   PlayerPaddle.Extent.GetLowerRight().x = WINDOW_LEFT_MARGIN + paddlePosition + Paddle::WIDTH;

   double remainingTime = frameTime;
   std::vector<Collision> collisions;

   while (remainingTime > .0 && AppState::Playing == CurrentAppState)
   {
      glm::vec2 newBallPosition = {
         PlayerBall.Position.x + static_cast<FLOAT>(PlayerBall.Velocity.x * remainingTime),
         PlayerBall.Position.y + static_cast<FLOAT>(PlayerBall.Velocity.y * remainingTime)
      };

      CheckForWallCollisions(collisions, newBallPosition);
      CheckForBrickCollisions(collisions, newBallPosition);
      CheckForPaddleCollisions(collisions, newBallPosition);

      if (collisions.empty())
      {
         PlayerBall.Position = newBallPosition;
         break;
      }

      std::sort(collisions.begin(), collisions.end(),
         [](const Collision& c1, const Collision& c2) { return c1.ImpactTime < c2.ImpactTime; });

      const Collision& impact = collisions.front();
      const double collisionTime = remainingTime * impact.ImpactTime;
      PlayerBall.Position += PlayerBall.Velocity * static_cast<float>(collisionTime);
      remainingTime -= collisionTime;

      HandleImpact(impact);
      collisions.clear();
   }
}


void Game::DrawGameEntities(HDC surfaceContext)
{
   MainWindowDrawManager.DrawPaddle(surfaceContext, PlayerPaddle);

   for (const auto& brick : CurrentLevel->Bricks)
      if (brick.Active)
         MainWindowDrawManager.DrawBrick(surfaceContext, brick);

   DrawManager::DrawBall(surfaceContext, PlayerBall.Position);
}


void Game::DrawPlayerScore(HDC surfaceContext)
{
   DrawManager::DrawString(surfaceContext, glm::vec2(800.f, 50.f), std::format("Score:\n{}", PlayerScore));
   DrawManager::DrawString(surfaceContext, glm::vec2(800.f, 150.f), std::format("Lives:\n{}", PlayerLives));
}


void Game::DrawReadyMessage(HDC surfaceContext)
{
   DrawManager::DrawString(surfaceContext, glm::vec2(800.f, 250.f), "Click left mouse button to start!");
}


void Game::DrawLevelCompletedMessage(HDC surfaceContext)
{
   DrawManager::DrawString(surfaceContext, glm::vec2(800.f, 250.f), 
      "Level completed!\nClick left mouse button\n to start next level!");
}


void Game::DrawGameOverMessage(HDC surfaceContext)
{
   DrawManager::DrawString(surfaceContext, glm::vec2(800.f, 50.f), 
      std::format("GAME OVER!\nFinal score: {}\n\nClick left mouse button\nto start a new game!", PlayerScore));
}


void Game::CheckForWallCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const
{
   if (PlayerBall.Velocity.x < .0)
   {
      if (newBallPosition.x < BALL_AREA.left)
      {
         const double impactTime = inverseLerp(PlayerBall.Position.x, newBallPosition.x, static_cast<float>(BALL_AREA.left));
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Left, impactTime);
      }
   }
   else
   {
      if (newBallPosition.x >= BALL_AREA.right)
      {
         const double impactTime = inverseLerp(PlayerBall.Position.x, newBallPosition.x, static_cast<float>(BALL_AREA.right));
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Right, impactTime);
      }
   }

   if (PlayerBall.Velocity.y < .0)
   {
      if (newBallPosition.y < BALL_AREA.top)
      {
         const double impactTime = inverseLerp(PlayerBall.Position.y, newBallPosition.y, static_cast<float>(BALL_AREA.top));
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Top, impactTime);
      }
   }
   else
   {
      if (newBallPosition.y >= BALL_AREA.bottom)
      {
         const double impactTime = inverseLerp(PlayerBall.Position.y, newBallPosition.y, static_cast<float>(BALL_AREA.bottom));
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Bottom, impactTime);
      }
   }
}


void Game::CheckForBrickCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition)
{
   const Box2D ballRectangle = Box2D(PlayerBall.Position, Ball::RADIUS) + Box2D(newBallPosition, Ball::RADIUS);

   for (auto& brick : CurrentLevel->Bricks)
   {
      if (!brick.Active || !ballRectangle.Intersects(brick.Extent))
         continue;

      CollisionSide collisionSide;
      double collisionTime = .0f;

      if (DetectBallVsRectangleCollision(PlayerBall.Position, newBallPosition, brick.Extent, 
         collisionSide, collisionTime))
         collisions.push_back(Collision(CollisionType::Brick, collisionSide, collisionTime, &brick));
   }
}


void Game::CheckForPaddleCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const
{
   const Box2D ballRectangle = Box2D(PlayerBall.Position, Ball::RADIUS) + Box2D(newBallPosition, Ball::RADIUS);

   if (!ballRectangle.Intersects(PlayerPaddle.Extent))
      return;

   CollisionSide collisionSide;
   double collisionTime = .0f;

   if (DetectBallVsRectangleCollision(PlayerBall.Position, newBallPosition, PlayerPaddle.Extent,
      collisionSide, collisionTime))
      collisions.push_back(Collision(CollisionType::Paddle, collisionSide, collisionTime));
}


void Game::HandleImpact(const Collision& impact)
{
   switch (impact.Type)
   {
      case CollisionType::Brick:
         HandleRectangleCollision(impact.TargetBrick->Extent, impact.Side);
         BrickImpact(*impact.TargetBrick);
         break;
      case CollisionType::Paddle:
         HandleRectangleCollision(PlayerPaddle.Extent, impact.Side);
         break;
      case CollisionType::Wall:
         HandleWallCollision(impact.Side);
         break;
   }
}


void Game::HandleRectangleCollision(const Box2D& extent, CollisionSide side)
{
   switch (side)
   {
      case CollisionSide::Bottom:
         PlayerBall.Position.y = extent.GetLowerRight().y + Ball::RADIUS;
         PlayerBall.Velocity.y *= -1.f;
         break;
      case CollisionSide::Left:
         PlayerBall.Position.x = extent.GetUpperLeft().x - Ball::RADIUS;
         PlayerBall.Velocity.x *= -1.f;
         break;
      case CollisionSide::Right:
         PlayerBall.Position.x = extent.GetLowerRight().x + Ball::RADIUS;
         PlayerBall.Velocity.x *= -1.f;
         break;
      case CollisionSide::Top:
         PlayerBall.Position.y = extent.GetUpperLeft().y - Ball::RADIUS;
         PlayerBall.Velocity.y *= -1.f;
         break;
      case CollisionSide::BottomLeft:
         PointBounce(glm::vec2(extent.GetUpperLeft().x, extent.GetLowerRight().y));
         break;
      case CollisionSide::BottomRight:
         PointBounce(extent.GetLowerRight());
         break;
      case CollisionSide::TopLeft:
         PointBounce(extent.GetUpperLeft());
         break;
      case CollisionSide::TopRight:
         PointBounce(glm::vec2(extent.GetLowerRight().x, extent.GetUpperLeft().y));
         break;
   }
}


void Game::PointBounce(const glm::vec2& point)
{
   const glm::vec2 pointToBallDirection = glm::normalize(PlayerBall.Position - point);
   PlayerBall.Position = point + pointToBallDirection * Ball::RADIUS;
   PlayerBall.Velocity = glm::reflect(PlayerBall.Velocity, pointToBallDirection);
}


void Game::HandleWallCollision(CollisionSide side)
{
   switch (side)
   {
      case CollisionSide::Left:
         PlayerBall.Position.x = static_cast<float>(BALL_AREA.left);
         PlayerBall.Velocity.x *= -1.f;
         break;
      case CollisionSide::Right:
         PlayerBall.Position.x = static_cast<float>(BALL_AREA.right);
         PlayerBall.Velocity.x *= -1.f;
         break;
      case CollisionSide::Top:
         PlayerBall.Position.y = static_cast<float>(BALL_AREA.top);
         PlayerBall.Velocity.y *= -1.f;
         break;
      case CollisionSide::Bottom:
         ChangeAppState((--PlayerLives == 0) ? AppState::Lost : AppState::Ready);
         break;
   }
}


void Game::BrickImpact(Brick& brick)
{
   brick.Active = false;
   PlayerScore++;
   CurrentLevel->ActiveBrickCount--;

   if (0 == CurrentLevel->ActiveBrickCount)
      ChangeAppState(AppState::LevelCompleted);
}


void Game::ChangeAppState(AppState newAppState)
{
   if (newAppState == CurrentAppState || AppState::Init == newAppState) // Transitioning back to Init is not allowed
      return;

   CurrentAppState = newAppState;

   if (AppState::Ready == CurrentAppState)
      ResetBall();

   if (AppState::Playing == CurrentAppState)
      while (ShowCursor(FALSE) >= 0);
   else
      ShowCursor(TRUE);
}


bool Game::DetectBallVsRectangleCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition,
   const Box2D& rectangleExtent, CollisionSide& out_collisionSide, double& out_collisionTime)
{
   const Box2D ballExtentStart(ballStartPosition, Ball::RADIUS);
   const Box2D ballExtentEnd(ballEndPosition, Ball::RADIUS);

   bool checkTopLeftCorner = true;
   bool checkTopRightCorner = true;
   bool checkBottomRightCorner = true;
   bool checkBottomLeftCorner = true;

   // Test for intersections with the horizontal edges of the rectangle
   if (ballExtentStart.GetUpperLeft().y >= rectangleExtent.GetLowerRight().y && ballExtentEnd.GetUpperLeft().y <= rectangleExtent.GetLowerRight().y)
   {
      const double timeFraction = inverseLerp(ballExtentStart.GetUpperLeft().y, ballExtentEnd.GetUpperLeft().y, rectangleExtent.GetLowerRight().y);
      const float xImpact = lerp(ballStartPosition.x, ballEndPosition.x, timeFraction);

      if (xImpact >= rectangleExtent.GetUpperLeft().x && xImpact <= rectangleExtent.GetLowerRight().x)
      {
         out_collisionSide = CollisionSide::Bottom;
         out_collisionTime = timeFraction;
         return true;
      }

      checkTopLeftCorner = false;
      checkTopRightCorner = false;
   }
   else if (ballExtentStart.GetLowerRight().y <= rectangleExtent.GetUpperLeft().y && ballExtentEnd.GetLowerRight().y >= rectangleExtent.GetUpperLeft().y)
   {
      const double timeFraction = inverseLerp(ballExtentStart.GetLowerRight().y, ballExtentEnd.GetLowerRight().y, rectangleExtent.GetUpperLeft().y);
      const float xImpact = lerp(ballStartPosition.x, ballEndPosition.x, timeFraction);

      if (xImpact > rectangleExtent.GetUpperLeft().x && xImpact < rectangleExtent.GetLowerRight().x)
      {
         out_collisionSide = CollisionSide::Top;
         out_collisionTime = timeFraction;
         return true;
      }

      checkBottomLeftCorner = false;
      checkBottomRightCorner = false;
   }

   // Test for intersections with the vertical edges of the rectangle
   if (ballExtentStart.GetUpperLeft().x >= rectangleExtent.GetLowerRight().x && ballExtentEnd.GetUpperLeft().x <= rectangleExtent.GetLowerRight().x)
   {
      const double timeFraction = inverseLerp(ballExtentStart.GetUpperLeft().x, ballExtentEnd.GetUpperLeft().x, rectangleExtent.GetLowerRight().x);
      const float yImpact = lerp(ballStartPosition.y, ballEndPosition.y, timeFraction);

      if (yImpact > rectangleExtent.GetUpperLeft().y && yImpact < rectangleExtent.GetLowerRight().y)
      {
         out_collisionSide = CollisionSide::Right;
         out_collisionTime = timeFraction;
         return true;
      }

      checkTopLeftCorner = false;
      checkBottomLeftCorner = false;
   }
   else if (ballExtentStart.GetLowerRight().x <= rectangleExtent.GetUpperLeft().x && ballExtentEnd.GetLowerRight().x >= rectangleExtent.GetUpperLeft().x)
   {
      const double timeFraction = inverseLerp(ballExtentStart.GetLowerRight().x, ballExtentEnd.GetLowerRight().x, rectangleExtent.GetUpperLeft().x);
      const float yImpact = lerp(ballStartPosition.y, ballEndPosition.y, timeFraction);

      if (yImpact > rectangleExtent.GetUpperLeft().y && yImpact < rectangleExtent.GetLowerRight().y)
      {
         out_collisionSide = CollisionSide::Left;
         out_collisionTime = timeFraction;
         return true;
      }

      checkTopRightCorner = false;
      checkBottomRightCorner = false;
   }

   // Finally, test for collisions against the remaining corners
   if (checkBottomLeftCorner && DetectBallVsPointCollision(ballStartPosition, ballEndPosition, 
      glm::vec2(rectangleExtent.GetUpperLeft().x, rectangleExtent.GetLowerRight().y), out_collisionTime))
   {
      out_collisionSide = CollisionSide::BottomLeft;
      return true;
   }

   if (checkBottomRightCorner && DetectBallVsPointCollision(ballStartPosition, ballEndPosition, 
      rectangleExtent.GetLowerRight(), out_collisionTime))
   {
      out_collisionSide = CollisionSide::BottomRight;
      return true;
   }

   if (checkTopRightCorner && DetectBallVsPointCollision(ballStartPosition, ballEndPosition, 
      glm::vec2(rectangleExtent.GetLowerRight().x, rectangleExtent.GetUpperLeft().y), out_collisionTime))
   {
      out_collisionSide = CollisionSide::TopRight;
      return true;
   }

   if (checkTopLeftCorner && DetectBallVsPointCollision(ballStartPosition, ballEndPosition, 
      rectangleExtent.GetUpperLeft(), out_collisionTime))
   {
      out_collisionSide = CollisionSide::TopLeft;
      return true;
   }

   return false;
}


bool Game::DetectBallVsPointCollision(const glm::vec2& ballStartPosition, const glm::vec2& ballEndPosition, 
   const glm::vec2& testPoint, double& out_collisionTime)
{
   const glm::vec2 delta = ballEndPosition - ballStartPosition;
   const glm::vec2 testPointDirection = testPoint - ballStartPosition;

   if (glm::dot(delta, testPointDirection) <= .0f)
      return false;

   const float a = glm::dot(delta, delta);
   const float b = 2.f * (glm::dot(ballStartPosition, delta) - glm::dot(testPoint, delta));
   
   const float c = glm::dot(ballStartPosition, ballStartPosition) + glm::dot(testPoint, testPoint)
      - 2.f * glm::dot(ballStartPosition, testPoint) - Ball::RADIUS * Ball::RADIUS;

   float result1, result2;

   if (solveQuadratic(a, b, c, result1, result2) == 2 && result1 >= .0f && result1< 1.f)
   {
      out_collisionTime = result1;
      return true;
   }

   return false;
}
