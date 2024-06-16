#include "WindowClass.h"

static constexpr int ARROW_CURSOR_RESOURCE_ID = 32512;
static constexpr int WINLOGO_ICON_RESOURCE_ID = 32517;
static constexpr int DEFAULT_STYLE = CS_HREDRAW | CS_VREDRAW;


WindowClass::WindowClass(HINSTANCE instance, const std::string& className, WNDPROC windowProc)
   : ClassName(className), Atom(0)
{
   ZeroMemory(&WndClass, sizeof(WNDCLASSEX));
   WndClass.cbSize = sizeof(WNDCLASSEX);
   WndClass.style = DEFAULT_STYLE;
   WndClass.lpfnWndProc = windowProc;
   WndClass.lpszClassName = ClassName.c_str();
   WndClass.hInstance = instance;
   WndClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
   WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
}


WindowClass::~WindowClass()
{
   Unregister();
}


ATOM WindowClass::GetAtom() const
{
   return Atom;
}


UINT WindowClass::GetStyle() const
{
   return WndClass.style;
}


void WindowClass::SetIcon(HICON iconHandle)
{
   WndClass.hIcon = iconHandle;
}


void WindowClass::SetCursor(HCURSOR cursorHandle)
{
   WndClass.hCursor = cursorHandle;
}


void WindowClass::SetStyle(UINT style)
{
   WndClass.style = style;
}


bool WindowClass::Register()
{
   if (0 != Atom)
      Unregister();

   Atom = RegisterClassEx(&WndClass);
   return 0 != Atom;
}


void WindowClass::Unregister()
{
   UnregisterClass(ClassName.c_str(), WndClass.hInstance);
}
