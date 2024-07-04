#pragma once

#include <windows.h>


constexpr float BALL_RADIUS = 8.f;


struct Brick
{
   RECT Extent;
   COLORREF Color;
   
   Brick(const RECT& extent, COLORREF color)
      : Extent(extent), Color(color)
   { }
};
