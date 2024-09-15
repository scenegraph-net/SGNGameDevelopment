#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include <glm/glm.hpp>

#include "BitmapImage.h"

struct Brick;
struct Paddle;

class Box2D;


class DrawManager
{
   public:
      DrawManager(int backgroundBitmapResourceId, int paddleBitmapResourceId);

      int AddBrickType(int brickBitmapResourceId);

      void DrawBackground(HDC deviceContext, const RECT& extent) const;
      void DrawBrick(HDC deviceContext, const Brick& brick);
      void DrawPaddle(HDC deviceContext, const Paddle& paddle);

      static void DrawBall(HDC deviceContext, const glm::vec2& ballPosition);
      static void DrawString(HDC deviceContext, const glm::vec2& position, const std::string& text);

   private:
      BitmapImage BackgroundImage;
      BitmapImage PaddleImage;
      std::vector<BitmapImage> BrickImages;

      static void ConvertFromBox2D(const Box2D& source, RECT& out_target);
};
