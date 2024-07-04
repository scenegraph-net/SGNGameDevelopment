#include "Game.h"
#include "DrawManager.h"

#include "GameEntities.h"


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ }), BallPosition({ 200.f, 400.f }),
   BallVelocity({ 150.f, 130.f })
{
   std::vector<COLORREF> colors = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
      RGB(0, 255, 255), RGB(127, 127, 127), RGB(255, 0, 255), RGB(255, 255, 0) };

   for (int y = 0; y < 8; y++)
      for (int x = 0; x < 15; x++)
         Bricks.emplace_back(RECT{ x * 48 + 24, y * 24 + 24, x * 48 + 72, y * 24 + 48 }, colors[(y * 15 + x) % colors.size()]);
}


void Game::SetMousePosition(int x, int y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void Game::Update(double frameTime)
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   constexpr int PADDLE_WIDTH = 96;
   constexpr int PADDLE_HEIGHT = 24;

   constexpr int LEFT_MARGIN = 24;
   constexpr int TOP_MARGIN = 24;
   
   constexpr int PLAYING_FIELD_WIDTH = 720;
   constexpr int PLAYING_FIELD_HEIGHT = 528;

   const RECT ballArea{
      LEFT_MARGIN + BALL_RADIUS,
      TOP_MARGIN + BALL_RADIUS,
      LEFT_MARGIN + PLAYING_FIELD_WIDTH - BALL_RADIUS,
      TOP_MARGIN + PLAYING_FIELD_HEIGHT - BALL_RADIUS
   };

   const int paddlePosition = min(max(0, MousePosition.x), PLAYING_FIELD_WIDTH - PADDLE_WIDTH);
   
   const RECT paddleExtent{
      LEFT_MARGIN + paddlePosition,
      TOP_MARGIN + PLAYING_FIELD_HEIGHT - PADDLE_HEIGHT,
      LEFT_MARGIN + paddlePosition + PADDLE_WIDTH,
      TOP_MARGIN + PLAYING_FIELD_HEIGHT
   };

   HDC surfaceContext = Surface.GetDeviceContext();
   DrawManager::DrawBackground(surfaceContext, clientRectangle);
   DrawManager::DrawPaddle(surfaceContext, paddleExtent);
   
   for (const auto& brick : Bricks)
      DrawManager::DrawBrick(surfaceContext, brick);

   BallPosition.x += static_cast<FLOAT>(BallVelocity.x * frameTime);
   BallPosition.y += static_cast<FLOAT>(BallVelocity.y * frameTime);

   if (BallPosition.x < ballArea.left)
   {
      BallPosition.x = ballArea.left;
      BallVelocity.x *= -1.f;
   }

   if (BallPosition.x >= ballArea.right)
   {
      BallPosition.x = ballArea.right - 1;
      BallVelocity.x *= -1.f;
   }

   if (BallPosition.y < ballArea.top)
   {
      BallPosition.y = ballArea.top;
      BallVelocity.y *= -1.f;
   }

   if (BallPosition.y >= ballArea.bottom)
   {
      BallPosition.y = ballArea.bottom - 1;
      BallVelocity.y *= -1.f;
   }

   DrawManager::DrawBall(surfaceContext, BallPosition);

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}
