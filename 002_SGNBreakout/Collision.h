#pragma once


enum class CollisionType { Wall, Brick, Paddle };
enum class CollisionSide { Top, Left, Right, Bottom, TopLeft, TopRight, BottomRight, BottomLeft };

struct Brick;


struct Collision
{
   CollisionType Type;
   CollisionSide Side;

   double ImpactTime;
   Brick* TargetBrick;

   Collision(CollisionType type, CollisionSide side, double impactTime, Brick* targetBlock = nullptr)
      : Type(type), Side(side), ImpactTime(impactTime), TargetBrick(targetBlock)
   { }
};
