#include "DrawManager.h"


void DrawManager::DrawBackground(HDC deviceContext, const RECT& extent)
{
   FillRect(deviceContext, &extent, static_cast<HBRUSH>(WHITE_BRUSH));
}


void DrawManager::DrawBrick(HDC deviceContext, const RECT& extent)
{
   Rectangle(deviceContext, extent.left, extent.top, extent.right, extent.bottom);
}


void DrawManager::DrawPaddle(HDC deviceContext, const RECT& extent)
{
   Rectangle(deviceContext, extent.left, extent.top, extent.right, extent.bottom);
}
