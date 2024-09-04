#include "Game.h"

#include <algorithm>
#include <format>

#include "DrawManager.h"
#include "MathUtils.h"


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
      const double collisionTime = remainingTime * impact.ImpactTime;
      PlayerBall.Position += PlayerBall.Velocity * static_cast<float>(collisionTime);
      remainingTime -= collisionTime;

      HandleImpact(impact);
      collisions.clear();
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
         PlayerBall.Position.y = BALL_AREA.bottom;
         PlayerBall.Velocity.y *= -1.f;
         break;
   }
}


void Game::BrickImpact(Brick& brick)
{
   // TODO: Implement
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
