#include <windows.h>
#include <windowsx.h>

#include <string>

#include "WindowClass.h"
#include "Game.h"


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   Game& game = *reinterpret_cast<Game*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

   switch (message)
   {
      case WM_MOUSEMOVE:
         game.SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
         return 0;
      case WM_SIZE:
         game.ResizeWindow(LOWORD(lParam), HIWORD(lParam));
         return 0;
      case WM_DESTROY:
         PostQuitMessage(0);
         return 0;
   }

   return DefWindowProc(windowHandle, message, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showMode)
{
   const std::string windowClassName = "SGNBreakout";
   const std::string mainWindowTitle = "SGNBreakout - a clone of the classic game";

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

   Game game(mainWindowHandle);
   SetWindowLongPtr(mainWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&game));

   ShowWindow(mainWindowHandle, showMode);
   UpdateWindow(mainWindowHandle);

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
         game.Update();
      }
   }

   return returnValue;
}