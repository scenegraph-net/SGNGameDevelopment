#pragma once

#include <windows.h>

#include <glm/glm.hpp>

struct Brick;
struct Paddle;

class Box2D;


class DrawManager
{
   public:
      static void DrawBackground(HDC deviceContext, const RECT& extent);
      static void DrawBrick(HDC deviceContext, const Brick& brick);
      static void DrawPaddle(HDC deviceContext, const Paddle& paddle);
      static void DrawBall(HDC deviceContext, const glm::vec2& ballPosition);

   private:
      static void ConvertFromBox2D(const Box2D& source, RECT& out_target);
};
