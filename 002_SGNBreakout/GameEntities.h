#pragma once

#include <windows.h>

#include <glm/glm.hpp>


struct Brick
{
   RECT Extent;
   COLORREF Color;
   
   Brick(const RECT& extent, COLORREF color)
      : Extent(extent), Color(color)
   { }
};


struct Paddle
{
   static constexpr int WIDTH = 96;
   static constexpr int HEIGHT = 24;

   LONG PositionX;
   RECT Extent;
};


struct Ball
{
   static constexpr float RADIUS = 8.f;

   glm::vec2 Position;
   glm::vec2 Velocity;
};
