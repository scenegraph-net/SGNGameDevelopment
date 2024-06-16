#pragma once

#include <string>
#include <windows.h>

#include "DrawingSurface.h"


class MainWindow
{
   public:
      MainWindow(HWND windowHandle);

      DrawingSurface& GetSurface();

      void GetClientArea(RECT& rectangle) const;
      std::string GetTitle() const;

      void Resize(unsigned short width, unsigned short height);
      void Present();

   private:
      HWND WindowHandle;
      DrawingSurface Surface;
};
