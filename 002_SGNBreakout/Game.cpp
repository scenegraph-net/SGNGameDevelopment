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

   std::vector<COLORREF> colors = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
      RGB(0, 255, 255), RGB(127, 127, 127), RGB(255, 0, 255), RGB(255, 255, 0) };

   for (int y = 0; y < 8; y++)
      for (int x = 0; x < 15; x++)
         Bricks.emplace_back(RECT{ x * 48 + 24, y * 24 + 24, x * 48 + 72, y * 24 + 48 }, colors[(y * 15 + x) % colors.size()]);
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
   const int paddlePosition = min(max(0, MousePosition.x), PLAYING_FIELD_WIDTH - Paddle::WIDTH);

   PlayerPaddle.Extent = {
      WINDOW_LEFT_MARGIN + paddlePosition,
      WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT - Paddle::HEIGHT,
      WINDOW_LEFT_MARGIN + paddlePosition + Paddle::WIDTH,
      WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT
   };

   double remainingTime = frameTime;
   std::vector<Collision> collisions;

   while (remainingTime > .0)
   {
      POINTFLOAT newBallPosition = {
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


void Game::CheckForWallCollisions(std::vector<Collision>& collisions, const POINTFLOAT& newBallPosition) const
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


void Game::CheckForBrickCollisions(std::vector<Collision>& collisions, const POINTFLOAT& newBallPosition) const
{
   // Do nothing for now
}


void Game::CheckForPaddleCollisions(std::vector<Collision>& collisions, const POINTFLOAT& newBallPosition) const
{
   // Do nothing for now
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


double Game::Length(const POINTFLOAT& target)
{
   return sqrt(target.x * target.x + target.y * target.y);
}
