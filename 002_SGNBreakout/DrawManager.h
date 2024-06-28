#pragma once

#include <windows.h>


class DrawManager
{
   public:
      static void DrawBackground(HDC deviceContext, const RECT& extent);
      static void DrawBrick(HDC deviceContext, const RECT& extent);
      static void DrawPaddle(HDC deviceContext, const RECT& extent);
};
