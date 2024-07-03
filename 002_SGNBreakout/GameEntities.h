#pragma once

#include <windows.h>


struct Brick
{
   RECT Extent;
   COLORREF Color;
   
   Brick(const RECT& extent, COLORREF color)
      : Extent(extent), Color(color)
   { }
};
