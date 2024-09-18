#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include <glm/glm.hpp>

#include "BitmapImage.h"

struct Brick;
struct Paddle;

class Box2D;


enum class TextType
{
   GameMessage,
   ScoreHeader,
   ScoreValue,
   LivesHeader,
   LivesValue,
   LevelHeader,
   LevelValue
};


class DrawManager
{
   public:
      DrawManager(int backgroundBitmapResourceId, int paddleBitmapResourceId);

      int AddBrickType(int brickBitmapResourceId);

      void DrawBackground(HDC deviceContext, const RECT& extent) const;
      void DrawBrick(HDC deviceContext, const Brick& brick);
      void DrawPaddle(HDC deviceContext, const Paddle& paddle);
      void DrawString(HDC deviceContext, TextType textType, const std::string& text);

      static void DrawBall(HDC deviceContext, const glm::vec2& ballPosition);

   private:
      BitmapImage BackgroundImage;
      BitmapImage PaddleImage;
      std::vector<BitmapImage> BrickImages;

      HFONT GameMessageFont;
      HFONT NumericHeaderFont;
      HFONT NumericValueFont;

      RECT GameMessageRect;
      RECT ScoreHeaderRect;
      RECT ScoreValueRect;
      RECT LivesHeaderRect;
      RECT LivesValueRect;
      RECT LevelHeaderRect;
      RECT LevelValueRect;

      static void ConvertFromBox2D(const Box2D& source, RECT& out_target);
};
