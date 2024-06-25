#include "DrawingSurface.h"


DrawingSurface::DrawingSurface(HWND windowHandle)
   : WindowHandle(windowHandle)
{
   RECT clientRect;
   GetClientRect(WindowHandle, &clientRect);

   const unsigned short windowWidth = static_cast<unsigned short>(clientRect.right - clientRect.left);
   const unsigned short windowHeight = static_cast<unsigned short>(clientRect.bottom - clientRect.top);

   CreateBackBuffer(windowWidth, windowHeight);
}


DrawingSurface::~DrawingSurface()
{
   DestroyBackBuffer();
}


HDC DrawingSurface::GetDeviceContext() const
{
   return DeviceContext;
}


void DrawingSurface::CreateBackBuffer(unsigned short width, unsigned short height)
{
   if (nullptr != DeviceContext)
      DestroyBackBuffer();

   Width = width;
   Height = height;

   HDC windowDeviceContext = GetDC(WindowHandle);

   DeviceContext = CreateCompatibleDC(windowDeviceContext);
   BufferBitmap = CreateCompatibleBitmap(windowDeviceContext, Width, Height);
   OldBufferBitmap = static_cast<HBITMAP>(SelectObject(DeviceContext, BufferBitmap));

   ReleaseDC(WindowHandle, windowDeviceContext);
}


void DrawingSurface::DestroyBackBuffer()
{
   if (nullptr == DeviceContext)
      return;

   HDC windowDeviceContext = GetDC(WindowHandle);
   SelectObject(windowDeviceContext, OldBufferBitmap);
   ReleaseDC(WindowHandle, windowDeviceContext);

   DeleteObject(BufferBitmap);
   DeleteDC(DeviceContext);

   BufferBitmap = nullptr;
   DeviceContext = nullptr;
}


void DrawingSurface::Present(HDC targetDeviceContext)
{
   BitBlt(targetDeviceContext, 0, 0, Width, Height, DeviceContext, 0, 0, SRCCOPY);
}
