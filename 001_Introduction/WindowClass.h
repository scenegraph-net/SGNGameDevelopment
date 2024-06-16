#pragma once

#include <string>
#include <windows.h>


class WindowClass
{
   public:
      WindowClass(HINSTANCE instance, const std::string& className, WNDPROC windowProc);
      ~WindowClass();

      bool Register();
      void Unregister();

      void SetIcon(HICON iconHandle);
      void SetCursor(HCURSOR cursorHandle);

      UINT GetStyle() const;
      void SetStyle(UINT style);

      ATOM GetAtom() const;

   private:
      const std::string ClassName;
      WNDCLASSEX WndClass;
      ATOM Atom;
};
