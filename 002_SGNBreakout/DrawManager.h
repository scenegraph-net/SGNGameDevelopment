#pragma once

#include <windows.h>

struct Brick;


class DrawManager
{
   public:
      static void DrawBackground(HDC deviceContext, const RECT& extent);
      static void DrawBrick(HDC deviceContext, const Brick& brick);
      static void DrawPaddle(HDC deviceContext, const RECT& extent);
      static void DrawBall(HDC deviceContext, const POINTFLOAT& ballPosition);
};
