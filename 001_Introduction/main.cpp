#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>

static float g_time = .0f;


void DrawToWindow(HWND windowHandle)
{
   RECT clientRect;
   PAINTSTRUCT paintStruct;

   HDC deviceContext = BeginPaint(windowHandle, &paintStruct);

   // Calculate displacements
   constexpr float AMPLITUDE = 100.f;
   constexpr float TIMESCALE = 3.f;
   const int displacement1 = static_cast<int>(AMPLITUDE + sin(g_time * TIMESCALE) * AMPLITUDE);
   const int displacement2 = static_cast<int>(AMPLITUDE + cos(g_time * TIMESCALE) * AMPLITUDE);

   // Draw white background
   GetClientRect(windowHandle, &clientRect);
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

   // Get the title of the window
   size_t titleLength = GetWindowTextLength(windowHandle);
   std::vector<char> windowTitle(titleLength + 1);
   GetWindowText(windowHandle, windowTitle.data(), static_cast<int>(titleLength + 1));

   // Create a font and use it for drawing the window title
   HFONT textFont = CreateFont(32, 20, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
      CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Arial");

   HFONT oldFont = static_cast<HFONT>(SelectObject(deviceContext, textFont));

   RECT textLocalRect;
   DrawText(deviceContext, windowTitle.data(), static_cast<int>(windowTitle.size()), &textLocalRect, DT_CALCRECT);

   int textWidth = textLocalRect.right - textLocalRect.left;
   int textHeight = textLocalRect.bottom - textLocalRect.top;

   RECT textDrawRect;
   textDrawRect.left = 20 + displacement1;
   textDrawRect.right = 20 + textWidth + displacement1;
   textDrawRect.top = 450 + displacement2;
   textDrawRect.bottom = 450 + textHeight + displacement2;

   DrawText(deviceContext, windowTitle.data(), static_cast<int>(windowTitle.size()), &textDrawRect, DT_CENTER);

   SelectObject(deviceContext, oldFont);
   DeleteObject(textFont);

   EndPaint(windowHandle, &paintStruct);
}


LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_PAINT:
         DrawToWindow(windowHandle);
         return 0;
      case WM_TIMER:
         g_time += .02f;
         InvalidateRect(windowHandle, nullptr, TRUE);
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

   SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

   constexpr int WINDOW_WIDTH = 1280;
   constexpr int WINDOW_HEIGHT = 720;
   constexpr DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW;
   constexpr DWORD WINDOW_EX_STYLE = 0;

   RECT windowArea = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
   AdjustWindowRectEx(&windowArea, WINDOW_STYLE, FALSE, WINDOW_EX_STYLE);

   const int adjustedWidth = windowArea.right - windowArea.left;
   const int adjustedHeight = windowArea.bottom - windowArea.top;

   HWND mainWindow = CreateWindowEx(WINDOW_EX_STYLE, windowClassName.c_str(), mainWindowTitle.c_str(), WINDOW_STYLE,
      CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight, nullptr, nullptr, instance, nullptr);

   if (nullptr == mainWindow)
   {
      MessageBox(nullptr, "Unable to create main window", "Error", MB_OK);
      return 0;
   }

   ShowWindow(mainWindow, showMode);
   UpdateWindow(mainWindow);

   SetTimer(mainWindow, 0, 20, nullptr);

   MSG message;

   while (GetMessage(&message, nullptr, 0, 0))
   {
      TranslateMessage(&message);
      DispatchMessage(&message);
   }

   return static_cast<int>(message.wParam);
}
