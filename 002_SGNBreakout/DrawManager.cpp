#include "DrawManager.h"

#include "GameEntities.h"
#include "BitmapImage.h"


DrawManager::DrawManager(int backgroundBitmapResourceId, int paddleBitmapResourceId)
{
   HINSTANCE instance = GetModuleHandle(nullptr);
   BackgroundImage.LoadFromResource(instance, backgroundBitmapResourceId);
   PaddleImage.LoadFromResource(instance, paddleBitmapResourceId);
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


void DrawManager::DrawString(HDC deviceContext, const glm::vec2& position, const std::string& text)
{
   RECT rectangle{ static_cast<int>(position.x), static_cast<int>(position.y), 0, 0 };
   DrawText(deviceContext, text.c_str(), static_cast<int>(text.size()), &rectangle, DT_CALCRECT);
   DrawText(deviceContext, text.c_str(), static_cast<int>(text.size()), &rectangle, 0);
}


void DrawManager::ConvertFromBox2D(const Box2D& source, RECT& out_target)
{
   out_target.left = static_cast<LONG>(source.GetUpperLeft().x);
   out_target.top = static_cast<LONG>(source.GetUpperLeft().y);
   out_target.right = static_cast<LONG>(source.GetLowerRight().x);
   out_target.bottom = static_cast<LONG>(source.GetLowerRight().y);
}
