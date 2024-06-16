#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

#include "WindowClass.h"
#include "MainWindow.h"

static float g_time = .0f;


void DrawToWindow(MainWindow& window)
{
   // Calculate displacements
   constexpr float AMPLITUDE = 100.f;
   constexpr float TIMESCALE = .25f;
   const int displacement1 = static_cast<int>(AMPLITUDE + sin(g_time * TIMESCALE) * AMPLITUDE);
   const int displacement2 = static_cast<int>(AMPLITUDE + cos(g_time * TIMESCALE) * AMPLITUDE);

   HDC deviceContext = window.GetSurface().GetDeviceContext();

   // Draw white background
   RECT clientRect;
   window.GetClientArea(clientRect);
   FillRect(deviceContext, &clientRect, static_cast<HBRUSH>(WHITE_BRUSH));

   // Draw outlined shapes
   Rectangle(deviceContext, 20 + displacement1, 20, 150 + displacement1, 150);
   Ellipse(deviceContext, 180 + displacement1, 20, 400 + displacement1, 150);

   // Draw a filled rectangle
   HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
   RECT redRectangle{ 20 + displacement2, 170, 150 + displacement2, 300 };
   FillRect(deviceContext, &redRectangle, redBrush);
   DeleteObject(redBrush);

   // Draw a filled ellipse
   HBRUSH blueBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 255));
   HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(deviceContext, blueBrush));
   Ellipse(deviceContext, 180 + displacement2, 170, 400 + displacement2, 300);
   SelectObject(deviceContext, oldBrush);
   DeleteObject(blueBrush);

   // Create a font and use it for drawing the window title
   HFONT textFont = CreateFont(32, 20, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext, textFont));

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

   DrawText(deviceContext, windowTitle.c_str(), static_cast<int>(windowTitle.length()), &textDrawRect, DT_CENTER);

   SelectObject(deviceContext, oldFont);
   DeleteObject(textFont);

   window.Present();
}


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   MainWindow& window = *reinterpret_cast<MainWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

   switch (message)
   {
      case WM_PAINT:
         DrawToWindow(window);
         return 0;
      case WM_ERASEBKGND:
         return TRUE;
      case WM_TIMER:
         g_time += .4f;
         InvalidateRect(windowHandle, nullptr, TRUE);
         return 0;
      case WM_SIZE:
         window.Resize(LOWORD(lParam), HIWORD(lParam));
         return 0;
      case WM_CREATE:
         SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(new MainWindow(windowHandle)));
         return 0;
      case WM_DESTROY:
         delete reinterpret_cast<MainWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
         PostQuitMessage(0);
         return 0;
   }

   return DefWindowProc(windowHandle, message, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showMode)
{
   const std::string windowClassName = "SGNGameDev";
   const std::string mainWindowTitle = "SGN Game Development application";

   WindowClass windowClass(instance, windowClassName, WindowProc);

   if (!windowClass.Register())
   {
      MessageBox(nullptr, "Unable to register window class", "Error", MB_OK);
      return 0;
   }

   HWND mainWindowHandle = CreateWindow(windowClassName.c_str(), mainWindowTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, nullptr);

   if (nullptr == mainWindowHandle)
   {
      MessageBox(nullptr, "Unable to create main window", "Error", MB_OK);
      return 0;
   }

   ShowWindow(mainWindowHandle, showMode);
   UpdateWindow(mainWindowHandle);

   SetTimer(mainWindowHandle, 0, 20, nullptr);

   MSG message;

   while (GetMessage(&message, nullptr, 0, 0))
   {
      TranslateMessage(&message);
      DispatchMessage(&message);
   }

   return static_cast<int>(message.wParam);
}
