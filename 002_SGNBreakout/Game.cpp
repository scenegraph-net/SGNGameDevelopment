#include "Game.h"

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

   FillRect(Surface.GetDeviceContext(), &clientRectangle, static_cast<HBRUSH>(WHITE_BRUSH));

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}
