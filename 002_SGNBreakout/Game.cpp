#include "Game.h"
#include "DrawManager.h"


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ })
{
}


void Game::SetMousePosition(unsigned short x, unsigned short y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void Game::ResizeWindow(unsigned short width, unsigned short height)
{
   Surface.CreateBackBuffer(width, height);
}


void Game::Update()
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   HDC surfaceContext = Surface.GetDeviceContext();
   DrawManager::DrawBackground(surfaceContext, clientRectangle);
   DrawManager::DrawPaddle(surfaceContext, RECT{ 100, 540, 200, 560 });
   
   for (int i = 0; i < 10; i++)
      DrawManager::DrawBrick(surfaceContext, RECT{ i * 50 + 10, 100, i * 50 + 50, 120 });

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}
