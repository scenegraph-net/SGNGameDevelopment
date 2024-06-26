#pragma once

#include <windows.h>

#include "HiResTimer.h"
#include "BitmapImage.h"

class MainWindow;


class DrawManager
{
   public:
      DrawManager(HINSTANCE instance);
      ~DrawManager();

      DrawManager(const DrawManager& other) = delete;
      DrawManager(DrawManager&& other) = delete;

      DrawManager& operator=(const DrawManager& other) = delete;
      DrawManager& operator=(DrawManager&& other) = delete;

      void SetMousePosition(LONG x, LONG y);

      void Draw(MainWindow& window);

   private:
      HiResTimer Timer;
      POINT MousePosition;

      HBRUSH RedSolidBrush;
      HBRUSH BlueHatchBrush;
      HBRUSH GreenSolidBrush;

      HFONT TextFont;

      BitmapImage BackgroundBitmap;
      BitmapImage SpriteBitmap;
};

