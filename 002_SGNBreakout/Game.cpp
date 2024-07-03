#include "Game.h"
#include "DrawManager.h"

#include "GameEntities.h"


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ })
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


void Game::Update()
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   constexpr int PADDLE_WIDTH = 96;
   constexpr int PADDLE_HEIGHT = 24;
   constexpr int PLAYING_FIELD_WIDTH = 720;
   constexpr int PLAYING_FIELD_HEIGHT = 528;

   const int paddlePosition = min(max(0, MousePosition.x), PLAYING_FIELD_WIDTH - PADDLE_WIDTH);
   
   const RECT paddleExtent{ 24 + paddlePosition, PLAYING_FIELD_HEIGHT - PADDLE_HEIGHT,
      24 + paddlePosition + PADDLE_WIDTH, PLAYING_FIELD_HEIGHT };

   HDC surfaceContext = Surface.GetDeviceContext();
   DrawManager::DrawBackground(surfaceContext, clientRectangle);
   DrawManager::DrawPaddle(surfaceContext, paddleExtent);
   
   for (const auto& brick : Bricks)
      DrawManager::DrawBrick(surfaceContext, brick);

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}
