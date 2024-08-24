#pragma once

#include <windows.h>

#include <glm/glm.hpp>

struct Brick;
struct Paddle;


class DrawManager
{
   public:
      static void DrawBackground(HDC deviceContext, const RECT& extent);
      static void DrawBrick(HDC deviceContext, const Brick& brick);
      static void DrawPaddle(HDC deviceContext, const Paddle& paddle);
      static void DrawBall(HDC deviceContext, const glm::vec2& ballPosition);
};
