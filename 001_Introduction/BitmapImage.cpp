#include "BitmapImage.h"


BitmapImage::BitmapImage()
   : Bitmap(nullptr), Width(0), Height(0)
{
}


BitmapImage::~BitmapImage()
{
   DeleteObject(Bitmap);
}


bool BitmapImage::LoadFromFile(const std::string& filePath)
{
   Bitmap = static_cast<HBITMAP>(LoadImage(nullptr, filePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
   return UpdateBitmapInfo();
}



bool BitmapImage::LoadFromResource(HINSTANCE instance, int resourceId)
{
   Bitmap = static_cast<HBITMAP>(LoadBitmap(instance, MAKEINTRESOURCE(resourceId)));
   return UpdateBitmapInfo();
}


bool BitmapImage::UpdateBitmapInfo()
{
   if (nullptr == Bitmap)
      return false;

   BITMAP bitmapInfo;
   GetObject(Bitmap, sizeof(BITMAP), &bitmapInfo);

   Width = bitmapInfo.bmWidth;
   Height = bitmapInfo.bmHeight;
   
   return true;
}


void BitmapImage::Stretch(HDC deviceContext, const RECT& rectangle) const
{
   if (0 == Width || 0 == Height)
      return;

   HDC bitmapContext = CreateCompatibleDC(deviceContext);
   HANDLE oldBitmap = SelectObject(bitmapContext, Bitmap);

   StretchBlt(deviceContext, rectangle.left, rectangle.top, rectangle.right - rectangle.left, rectangle.bottom - rectangle.top, 
      bitmapContext, 0, 0, Width, Height, SRCCOPY);

   SelectObject(bitmapContext, oldBitmap);
   DeleteDC(bitmapContext);
}
