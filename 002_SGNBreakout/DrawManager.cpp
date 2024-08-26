#include "DrawManager.h"

#include "GameEntities.h"


void DrawManager::DrawBackground(HDC deviceContext, const RECT& extent)
{
   FillRect(deviceContext, &extent, static_cast<HBRUSH>(WHITE_BRUSH));

   HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
   RECT playingField{ 24, 24, 744, 552 };
   FillRect(deviceContext, &playingField, blackBrush);
   DeleteObject(blackBrush);
}


void DrawManager::DrawBrick(HDC deviceContext, const Brick& brick)
{
   RECT brickExtent;
   ConvertFromBox2D(brick.Extent, brickExtent);
   HBRUSH brush = CreateSolidBrush(brick.Color);
   FillRect(deviceContext, &brickExtent, brush);
   DeleteObject(brush);
}


void DrawManager::DrawPaddle(HDC deviceContext, const Paddle& paddle)
{
   RECT paddleExtent;
   ConvertFromBox2D(paddle.Extent, paddleExtent);
   HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
   FillRect(deviceContext, &paddleExtent, yellowBrush);
   DeleteObject(yellowBrush);
}


void DrawManager::DrawBall(HDC deviceContext, const glm::vec2& ballPosition)
{
   Ellipse(deviceContext, 
      static_cast<LONG>(ballPosition.x - Ball::RADIUS),
      static_cast<LONG>(ballPosition.y - Ball::RADIUS),
      static_cast<LONG>(ballPosition.x + Ball::RADIUS),
      static_cast<LONG>(ballPosition.y + Ball::RADIUS)
   );
}


void DrawManager::ConvertFromBox2D(const Box2D& source, RECT& out_target)
{
   out_target.left = static_cast<LONG>(source.GetUpperLeft().x);
   out_target.top = static_cast<LONG>(source.GetUpperLeft().y);
   out_target.right = static_cast<LONG>(source.GetLowerRight().x);
   out_target.bottom = static_cast<LONG>(source.GetLowerRight().y);
}
