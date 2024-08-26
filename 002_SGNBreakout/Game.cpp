#include "Game.h"

#include <algorithm>

#include "DrawManager.h"


static constexpr int WINDOW_LEFT_MARGIN = 24;
static constexpr int WINDOW_TOP_MARGIN = 24;

static constexpr int PLAYING_FIELD_WIDTH = 720;
static constexpr int PLAYING_FIELD_HEIGHT = 528;

static constexpr RECT BALL_AREA =
{
   static_cast<LONG>(WINDOW_LEFT_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_LEFT_MARGIN + PLAYING_FIELD_WIDTH - Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT - Ball::RADIUS)
};


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ })
{
   PlayerBall.Position = { 200.f, 400.f };
   PlayerBall.Velocity = { 150.f, 150.f };

   PlayerPaddle.PositionX = 0;
   PlayerPaddle.Extent.GetUpperLeft().y = static_cast<float>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT) - Paddle::HEIGHT;
   PlayerPaddle.Extent.GetLowerRight().y = static_cast<float>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT);

   std::vector<COLORREF> colors = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
      RGB(0, 255, 255), RGB(127, 127, 127), RGB(255, 0, 255), RGB(255, 255, 0) };

   for (int y = 0; y < 8; y++)
   {
      for (int x = 0; x < 15; x++)
      {
         const Box2D brickExtent(
            glm::vec2(static_cast<float>(x * 48 + 24), static_cast<float>(y * 24 + 24)),
            glm::vec2(static_cast<float>(x * 48 + 72), static_cast<float>(y * 24 + 48))
         );

         Bricks.emplace_back(brickExtent, colors[(y * 15 + x) % colors.size()]);
      }
   }
}


void Game::SetMousePosition(int x, int y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void Game::Update(double frameTime)
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   UpdateGameState(frameTime);

   HDC surfaceContext = Surface.GetDeviceContext();
   DrawManager::DrawBackground(surfaceContext, clientRectangle);
   DrawGameEntities(surfaceContext);

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

   while (remainingTime > .0)
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
      PlayerBall.Position.x += static_cast<FLOAT>(PlayerBall.Velocity.x * impact.ImpactTime * remainingTime);
      PlayerBall.Position.y += static_cast<FLOAT>(PlayerBall.Velocity.y * impact.ImpactTime * remainingTime);
      remainingTime -= impact.ImpactTime;

      HandleImpact(impact);
   }
}


void Game::DrawGameEntities(HDC surfaceContext)
{
   DrawManager::DrawPaddle(surfaceContext, PlayerPaddle);

   for (const auto& brick : Bricks)
      DrawManager::DrawBrick(surfaceContext, brick);

   DrawManager::DrawBall(surfaceContext, PlayerBall.Position);
}


void Game::CheckForWallCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition) const
{
   if (PlayerBall.Velocity.x < .0)
   {
      if (newBallPosition.x < BALL_AREA.left)
      {
         const double impactTime = (PlayerBall.Position.x - BALL_AREA.left) / (PlayerBall.Position.x - newBallPosition.x);
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Left, impactTime);
      }
   }
   else
   {
      if (newBallPosition.x >= BALL_AREA.right)
      {
         const double impactTime = (BALL_AREA.right - PlayerBall.Position.x) / (newBallPosition.x - PlayerBall.Position.x);
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Right, impactTime);
      }
   }

   if (PlayerBall.Velocity.y < .0)
   {
      if (newBallPosition.y < BALL_AREA.top)
      {
         double impactTime = (PlayerBall.Position.y - BALL_AREA.top) / (PlayerBall.Position.y - newBallPosition.y);
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Top, impactTime);
      }
   }
   else
   {
      if (newBallPosition.y >= BALL_AREA.bottom)
      {
         double impactTime = (BALL_AREA.bottom - PlayerBall.Position.y) / (newBallPosition.y - PlayerBall.Position.y);
         collisions.emplace_back(CollisionType::Wall, CollisionSide::Bottom, impactTime);
      }
   }
}


void Game::CheckForBrickCollisions(std::vector<Collision>& collisions, const glm::vec2& newBallPosition)
{
   const Box2D ballRectangle = Box2D(PlayerBall.Position, Ball::RADIUS) + Box2D(newBallPosition, Ball::RADIUS);

   for (auto& brick : Bricks)
   {
      if (!ballRectangle.Intersects(brick.Extent))
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
      case CollisionType::Wall:
         switch (impact.Side)
         {
            case CollisionSide::Left:
            case CollisionSide::Right:
               PlayerBall.Velocity.x *= -1.;
               break;
            case CollisionSide::Top:
            case CollisionSide::Bottom:
               PlayerBall.Velocity.y *= -1.;
               break;
         }

         break;
      default:
         // Do nothing for now
         break;
   }
}


bool Game::DetectBallVsRectangleCollision(const glm::vec2& ballPosition0, const glm::vec2& ballPosition1,
   const Box2D& rectangleExtent, CollisionSide& out_collisionSide, double& our_collisionTime)
{
   // TODO: Implement

   return false;
}
