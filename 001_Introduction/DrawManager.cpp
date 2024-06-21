#include "DrawManager.h"

#include "MainWindow.h"
#include <cmath>


DrawManager::DrawManager()
{
   RedSolidBrush = CreateSolidBrush(RGB(255, 0, 0));
   BlueHatchBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 255));
   GreenSolidBrush = CreateSolidBrush(RGB(0, 255, 0));

   TextFont = CreateFont(32, 20, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   SpriteBitmap.LoadFromFile("assets\\woodwall.bmp");
   BackgroundBitmap.LoadFromFile("assets\\background.bmp");
}


DrawManager::~DrawManager()
{
   DeleteObject(RedSolidBrush);
   DeleteObject(BlueHatchBrush);
   DeleteObject(GreenSolidBrush);
   DeleteObject(TextFont);
}


void DrawManager::SetMousePosition(LONG x, LONG y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void DrawManager::Draw(MainWindow& window)
{
   // Calculate displacements
   constexpr double AMPLITUDE = 100.f;
   constexpr double TIMESCALE = 2.25f;

   const double elapsedTime = Timer.GetElapsed();
   const int displacement1 = static_cast<int>(AMPLITUDE + std::sin(elapsedTime * TIMESCALE) * AMPLITUDE);
   const int displacement2 = static_cast<int>(AMPLITUDE + std::cos(elapsedTime * TIMESCALE) * AMPLITUDE);

   HDC deviceContext = window.GetSurface().GetDeviceContext();

   // Draw background image
   RECT clientRect;
   window.GetClientArea(clientRect);
   BackgroundBitmap.Stretch(deviceContext, clientRect);

   // Draw outlined shapes
   HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(deviceContext, GetStockObject(NULL_BRUSH)));
   Rectangle(deviceContext, 20 + displacement1, 20, 150 + displacement1, 150);
   Ellipse(deviceContext, 180 + displacement1, 20, 400 + displacement1, 150);

   // Draw a filled rectangle
   RECT redRectangle{ 20 + displacement2, 170, 150 + displacement2, 300 };
   FillRect(deviceContext, &redRectangle, RedSolidBrush);

   // Draw a filled ellipse
   SelectObject(deviceContext, BlueHatchBrush);
   Ellipse(deviceContext, 180 + displacement2, 170, 400 + displacement2, 300);

   // Draw a circle that follows the mouse pointer
   constexpr int RADIUS = 15;
   SelectObject(deviceContext, GreenSolidBrush);

   Ellipse(deviceContext, MousePosition.x - RADIUS, MousePosition.y - RADIUS,
      MousePosition.x + RADIUS, MousePosition.y + RADIUS);

   SelectObject(deviceContext, oldBrush);

   // Draw the window title
   HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext, TextFont));

   std::string windowTitle = window.GetTitle();

   RECT textLocalRect;
   DrawText(deviceContext, windowTitle.c_str(), static_cast<int>(windowTitle.length()), &textLocalRect, DT_CALCRECT);

   int textWidth = textLocalRect.right - textLocalRect.left;
   int textHeight = textLocalRect.bottom - textLocalRect.top;

   RECT textDrawRect;
   textDrawRect.left = 20 + displacement1;
   textDrawRect.right = 20 + textWidth + displacement1;
   textDrawRect.top = 450 + displacement2;
   textDrawRect.bottom = 450 + textHeight + displacement2;

   int oldBackgroundMode = SetBkMode(deviceContext, TRANSPARENT);
   DrawText(deviceContext, windowTitle.c_str(), static_cast<int>(windowTitle.length()), &textDrawRect, DT_CENTER);
   SetBkMode(deviceContext, oldBackgroundMode);

   SelectObject(deviceContext, oldFont);

   // Draw the sprite
   RECT spriteRectangle{ MousePosition.x, MousePosition.y, MousePosition.x + 100, MousePosition.y + 100 };
   SpriteBitmap.Stretch(deviceContext, spriteRectangle);

   window.Present();
}
