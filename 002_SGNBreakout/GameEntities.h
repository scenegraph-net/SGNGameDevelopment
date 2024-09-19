#pragma once

#include <vector>

#include <windows.h>

#include <glm/glm.hpp>

#include "Box2D.h"


struct BrickType
{
   int ImageIndex;

   BrickType(int imageIndex)
      : ImageIndex(imageIndex)
   { }
};


struct Brick
{
   const BrickType& Type;
   Box2D Extent;
   bool Active;
   
   Brick(const Box2D& extent, const BrickType& type)
      : Extent(extent), Type(type), Active(true)
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

   float Speed;

   glm::vec2 Position;
   glm::vec2 Direction;
};


struct Level
{
   std::vector<Brick> Bricks;
   size_t ActiveBrickCount;
};
