#pragma once

#include <windows.h>


class DrawingSurface
{
   public:
      DrawingSurface(HWND windowHandle);
      ~DrawingSurface();

      void CreateBackBuffer(unsigned short width, unsigned short height);
      void DestroyBackBuffer();

      HDC GetDeviceContext() const;

      void Present(HDC targetDeviceContext);

   private:
      HWND WindowHandle;

      HDC DeviceContext;
      HBITMAP BufferBitmap;
      HBITMAP OldBufferBitmap;

      unsigned short Width;
      unsigned short Height;
};
