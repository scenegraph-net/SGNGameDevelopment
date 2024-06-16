#include "MainWindow.h"

#include <vector>


MainWindow::MainWindow(HWND windowHandle)
   : WindowHandle(windowHandle), Surface(windowHandle)
{
}


DrawingSurface& MainWindow::GetSurface()
{
   return Surface;
}


void MainWindow::GetClientArea(RECT& rectangle) const
{
   GetClientRect(WindowHandle, &rectangle);
}


std::string MainWindow::GetTitle() const
{
   size_t titleLength = GetWindowTextLength(WindowHandle);
   std::vector<char> windowTitle(titleLength + 1);
   GetWindowText(WindowHandle, windowTitle.data(), static_cast<int>(titleLength + 1));
   return windowTitle.data();
}


void MainWindow::Resize(unsigned short width, unsigned short height)
{
   Surface.CreateBackBuffer(width, height);
}


void MainWindow::Present()
{
   PAINTSTRUCT paintStruct;
   HDC windowDeviceContext = BeginPaint(WindowHandle, &paintStruct);
   Surface.Present(windowDeviceContext);
   EndPaint(WindowHandle, &paintStruct);
}
