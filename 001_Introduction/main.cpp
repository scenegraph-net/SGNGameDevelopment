#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   RECT clientRect;
   HDC deviceContext;
   PAINTSTRUCT paintStruct;

   switch (message)
   {
      case WM_PAINT:
         deviceContext = BeginPaint(windowHandle, &paintStruct);
         GetClientRect(windowHandle, &clientRect);
         FillRect(deviceContext, &clientRect, static_cast<HBRUSH>(WHITE_BRUSH));
         EndPaint(windowHandle, &paintStruct);
         return 0;
      case WM_DESTROY:
         PostQuitMessage(0);
         return 0;
   }

   return DefWindowProc(windowHandle, message, wParam, lParam);
}


int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showMode)
{
   const std::string windowClassName = "SGNGameDev";
   const std::string mainWindowTitle = "SGN Game Development application";

   WNDCLASSEX windowClass{ };
   windowClass.cbSize = sizeof(WNDCLASSEX);
   windowClass.style = CS_HREDRAW | CS_VREDRAW;
   windowClass.lpfnWndProc = static_cast<WNDPROC>(WindowProc);
   windowClass.hInstance = instance;
   windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
   windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
   windowClass.lpszClassName = windowClassName.c_str();

   if (!RegisterClassEx(&windowClass))
   {
      MessageBox(nullptr, "Unable to register window class", "Error", MB_OK);
      return 0;
   }

   HWND mainWindow = CreateWindow(windowClassName.c_str(), mainWindowTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, instance, nullptr);

   if (nullptr == mainWindow)
   {
      MessageBox(nullptr, "Unable to create main window", "Error", MB_OK);
      return 0;
   }

   ShowWindow(mainWindow, showMode);
   UpdateWindow(mainWindow);

   MSG message;

   while (GetMessage(&message, nullptr, 0, 0))
   {
      TranslateMessage(&message);
      DispatchMessage(&message);
   }

   return static_cast<int>(message.wParam);
}
