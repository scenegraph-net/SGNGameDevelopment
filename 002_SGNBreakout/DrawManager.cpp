#include "DrawManager.h"

#include "GameEntities.h"
#include "BitmapImage.h"


DrawManager::DrawManager(int backgroundBitmapResourceId, int paddleBitmapResourceId)
{
   HINSTANCE instance = GetModuleHandle(nullptr);
   BackgroundImage.LoadFromResource(instance, backgroundBitmapResourceId);
   PaddleImage.LoadFromResource(instance, paddleBitmapResourceId);

   GameMessageFont = CreateFont(24, 12, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   NumericHeaderFont = CreateFont(16, 8, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   NumericValueFont = CreateFont(30, 16, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   GameMessageRect = { 25, 285, 745, 381 };
   ScoreHeaderRect = { 817, 73, 961, 105 };
   ScoreValueRect = { 817, 99, 961, 165 };;
   LivesHeaderRect = { 817, 193, 961, 225 };;
   LivesValueRect = { 817, 219, 961, 285 };;
   LevelHeaderRect = { 817, 313, 961, 345 };;
   LevelValueRect = { 817, 339, 961, 405 };;
}


int DrawManager::AddBrickType(int brickBitmapResourceId)
{
   size_t result = BrickImages.size();
   BrickImages.emplace_back();
   BrickImages.back().LoadFromResource(GetModuleHandle(nullptr), brickBitmapResourceId);
   return static_cast<int>(result);
}


void DrawManager::DrawBackground(HDC deviceContext, const RECT& extent) const
{
   BackgroundImage.Stretch(deviceContext, extent);
}


void DrawManager::DrawBrick(HDC deviceContext, const Brick& brick)
{
   RECT brickExtent;
   ConvertFromBox2D(brick.Extent, brickExtent);
   BrickImages[brick.Type.ImageIndex].Stretch(deviceContext, brickExtent);
}


void DrawManager::DrawPaddle(HDC deviceContext, const Paddle& paddle)
{
   RECT paddleExtent;
   ConvertFromBox2D(paddle.Extent, paddleExtent);
   PaddleImage.Stretch(deviceContext, paddleExtent);
}


void DrawManager::DrawBall(HDC deviceContext, const glm::vec2& ballPosition)
{
   Ellipse(deviceContext, 
      static_cast<LONG>(ballPosition.x - Ball::RADIUS),
      static_cast<LONG>(ballPosition.y - Ball::RADIUS),
      static_cast<LONG>(ballPosition.x + Ball::RADIUS),
      static_cast<LONG>(ballPosition.y + Ball::RADIUS)
   );
}


void DrawManager::DrawString(HDC deviceContext, TextType textType, const std::string& text)
{
   RECT* textRectangle = nullptr;
   HFONT textFont = nullptr;
   UINT textFormat = 0;

   switch (textType)
   {
      case TextType::GameMessage:
         textRectangle = &GameMessageRect;
         textFont = GameMessageFont;
         textFormat = DT_CENTER;
         break;
      case TextType::ScoreHeader:
         textRectangle = &ScoreHeaderRect;
         textFont = NumericHeaderFont;
         textFormat = DT_LEFT;
         break;
      case TextType::ScoreValue:
         textRectangle = &ScoreValueRect;
         textFont = NumericValueFont;
         textFormat = DT_LEFT;
         break;
      case TextType::LivesHeader:
         textRectangle = &LivesHeaderRect;
         textFont = NumericHeaderFont;
         textFormat = DT_LEFT;
         break;
      case TextType::LivesValue:
         textRectangle = &LivesValueRect;
         textFont = NumericValueFont;
         textFormat = DT_LEFT;
         break;
      case TextType::LevelHeader:
         textRectangle = &LevelHeaderRect;
         textFont = NumericHeaderFont;
         textFormat = DT_LEFT;
         break;
      case TextType::LevelValue:
         textRectangle = &LevelValueRect;
         textFont = NumericValueFont;
         textFormat = DT_LEFT;
         break;
   }

   COLORREF oldColor = SetTextColor(deviceContext, RGB(255, 255, 255));
   int oldBackgroundMode = SetBkMode(deviceContext, TRANSPARENT);
   HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext, textFont));

   DrawText(deviceContext, text.c_str(), static_cast<int>(text.length()), textRectangle, textFormat);

   SelectObject(deviceContext, oldFont);
   SetBkMode(deviceContext, oldBackgroundMode);
   SetTextColor(deviceContext, oldColor);
}


void DrawManager::ConvertFromBox2D(const Box2D& source, RECT& out_target)
{
   out_target.left = static_cast<LONG>(source.GetUpperLeft().x);
   out_target.top = static_cast<LONG>(source.GetUpperLeft().y);
   out_target.right = static_cast<LONG>(source.GetLowerRight().x);
   out_target.bottom = static_cast<LONG>(source.GetLowerRight().y);
}
