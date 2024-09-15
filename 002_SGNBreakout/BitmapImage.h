#pragma once

#include <string>
#include <windows.h>


class BitmapImage
{
   public:
      BitmapImage();
      ~BitmapImage();

      BitmapImage(const BitmapImage& other) = delete;
      BitmapImage(BitmapImage&& other);

      BitmapImage& operator=(const BitmapImage& other) = delete;
      BitmapImage& operator=(BitmapImage&& other) = delete;

      bool LoadFromFile(const std::string& filePath);
      bool LoadFromResource(HINSTANCE instance, int resourceId);

      void Stretch(HDC deviceContext, const RECT& rectangle) const;

   private:
      HBITMAP Bitmap;
      LONG Width;
      LONG Height;

      bool UpdateBitmapInfo();
};
