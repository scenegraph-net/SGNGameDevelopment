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
   HBRUSH brush = CreateSolidBrush(brick.Color);
   FillRect(deviceContext, &brick.Extent, brush);
   DeleteObject(brush);
}


void DrawManager::DrawPaddle(HDC deviceContext, const RECT& extent)
{
   HBRUSH yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
   FillRect(deviceContext, &extent, yellowBrush);
   DeleteObject(yellowBrush);
}


void DrawManager::DrawBall(HDC deviceContext, const POINTFLOAT& ballPosition)
{
   Ellipse(deviceContext, 
      static_cast<LONG>(ballPosition.x - BALL_RADIUS),
      static_cast<LONG>(ballPosition.y - BALL_RADIUS),
      static_cast<LONG>(ballPosition.x + BALL_RADIUS),
      static_cast<LONG>(ballPosition.y + BALL_RADIUS)
   );
}
