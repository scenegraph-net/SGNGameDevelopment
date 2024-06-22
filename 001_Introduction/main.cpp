#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <memory>
#include <string>

#include "WindowClass.h"
#include "MainWindow.h"
#include "DrawManager.h"


static std::unique_ptr<DrawManager> g_drawManager;


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   MainWindow& window = *reinterpret_cast<MainWindow*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));

   switch (message)
   {
      case WM_MOUSEMOVE:
         g_drawManager->SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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

   g_drawManager = std::make_unique<DrawManager>(instance);

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
         MainWindow& window = *reinterpret_cast<MainWindow*>(GetWindowLongPtr(mainWindowHandle, GWLP_USERDATA));
         g_drawManager->Draw(window);
      }
   }

   return returnValue;
}
