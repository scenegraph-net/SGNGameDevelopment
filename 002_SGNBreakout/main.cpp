#include <windows.h>
#include <windowsx.h>

#include <string>

#include "WindowClass.h"
#include "Game.h"


//#define FIXED_FRAME_TIME

#ifdef FIXED_FRAME_TIME
   static constexpr double FRAME_TIME = .00125;
#else
   #include "HiResTimer.h"
#endif


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   Game& game = *reinterpret_cast<Game*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

   switch (message)
   {
      case WM_MOUSEMOVE:
         game.SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
         return 0;
      case WM_DESTROY:
         PostQuitMessage(0);
         return 0;
   }

   return DefWindowProc(windowHandle, message, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showMode)
{
   constexpr int MAIN_WINDOW_WIDTH = 1032;
   constexpr int MAIN_WINDOW_HEIGHT = 576;
   constexpr DWORD MAIN_WINDOW_STYLE = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);

   const std::string windowClassName = "SGNBreakout";
   const std::string mainWindowTitle = "SGNBreakout - a clone of the classic game";

   WindowClass windowClass(instance, windowClassName, WindowProc);

   if (!windowClass.Register())
   {
      MessageBox(nullptr, "Unable to register window class", "Error", MB_OK);
      return 0;
   }

   RECT windowRectangle{ 0, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT };
   AdjustWindowRect(&windowRectangle, MAIN_WINDOW_STYLE, FALSE);

   HWND mainWindowHandle = CreateWindow(windowClassName.c_str(), mainWindowTitle.c_str(), MAIN_WINDOW_STYLE, CW_USEDEFAULT,
      CW_USEDEFAULT, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, nullptr, 
      nullptr, instance, nullptr);

   if (nullptr == mainWindowHandle)
   {
      MessageBox(nullptr, "Unable to create main window", "Error", MB_OK);
      return 0;
   }

   Game game(mainWindowHandle);
   SetWindowLongPtr(mainWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&game));

   ShowWindow(mainWindowHandle, showMode);
   UpdateWindow(mainWindowHandle);

#ifndef FIXED_FRAME_TIME
   HiResTimer timer;
   double previousTime = timer.GetElapsed();
#endif

   MSG message;
   int returnValue = INT_MIN;

   while (INT_MIN == returnValue)
   {
      if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
      {
         if (message.message == WM_QUIT)
            returnValue = static_cast<int>(message.wParam);

         TranslateMessage(&message);
         DispatchMessage(&message);
      }
      else
      {
#ifdef FIXED_FRAME_TIME
         double frameTime = FRAME_TIME;
#else
         double elapsedTime = timer.GetElapsed();
         double frameTime = elapsedTime - previousTime;
         previousTime = elapsedTime;
#endif

         game.Update(frameTime);
      }
   }

   return returnValue;
}
