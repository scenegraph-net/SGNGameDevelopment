#pragma once

#include <windows.h>

#include <glm/glm.hpp>

#include "Box2D.h"


struct Brick
{
   Box2D Extent;
   COLORREF Color;
   bool Active;
   
   Brick(const Box2D& extent, COLORREF color)
      : Extent(extent), Color(color), Active(true)
   { }
};


struct Paddle
{
   static constexpr float WIDTH = 96.f;
   static constexpr float HEIGHT = 24.f;

   Box2D Extent;
   float PositionX;
};


struct Ball
{
   static constexpr float RADIUS = 8.f;

   glm::vec2 Position;
   glm::vec2 Velocity;
};
