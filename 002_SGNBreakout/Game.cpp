#include "Game.h"
#include "DrawManager.h"

#include "GameEntities.h"


static constexpr int WINDOW_LEFT_MARGIN = 24;
static constexpr int WINDOW_TOP_MARGIN = 24;

static constexpr int PLAYING_FIELD_WIDTH = 720;
static constexpr int PLAYING_FIELD_HEIGHT = 528;

static constexpr RECT BALL_AREA =
{
   static_cast<LONG>(WINDOW_LEFT_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + Ball::RADIUS),
   static_cast<LONG>(WINDOW_LEFT_MARGIN + PLAYING_FIELD_WIDTH - Ball::RADIUS),
   static_cast<LONG>(WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT - Ball::RADIUS)
};


Game::Game(HWND windowHandle)
   : Surface(windowHandle), WindowHandle(windowHandle), MousePosition({ })
{
   PlayerBall.Position = { 200.f, 400.f };
   PlayerBall.Velocity = { 150.f, 150.f };

   std::vector<COLORREF> colors = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
      RGB(0, 255, 255), RGB(127, 127, 127), RGB(255, 0, 255), RGB(255, 255, 0) };

   for (int y = 0; y < 8; y++)
      for (int x = 0; x < 15; x++)
         Bricks.emplace_back(RECT{ x * 48 + 24, y * 24 + 24, x * 48 + 72, y * 24 + 48 }, colors[(y * 15 + x) % colors.size()]);
}


void Game::SetMousePosition(int x, int y)
{
   MousePosition.x = x;
   MousePosition.y = y;
}


void Game::Update(double frameTime)
{
   RECT clientRectangle;
   GetClientRect(WindowHandle, &clientRectangle);

   UpdateGameState(frameTime);

   HDC surfaceContext = Surface.GetDeviceContext();
   DrawManager::DrawBackground(surfaceContext, clientRectangle);
   DrawGameEntities(surfaceContext);

   HDC deviceContext = GetDC(WindowHandle);
   Surface.Present(deviceContext);
   ReleaseDC(WindowHandle, deviceContext);
}


void Game::UpdateGameState(double frameTime)
{
   const int paddlePosition = min(max(0, MousePosition.x), PLAYING_FIELD_WIDTH - Paddle::WIDTH);

   PlayerPaddle.Extent = {
      WINDOW_LEFT_MARGIN + paddlePosition,
      WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT - Paddle::HEIGHT,
      WINDOW_LEFT_MARGIN + paddlePosition + Paddle::WIDTH,
      WINDOW_TOP_MARGIN + PLAYING_FIELD_HEIGHT
   };

   PlayerBall.Position.x += static_cast<FLOAT>(PlayerBall.Velocity.x * frameTime);
   PlayerBall.Position.y += static_cast<FLOAT>(PlayerBall.Velocity.y * frameTime);

   if (PlayerBall.Position.x < BALL_AREA.left)
   {
      PlayerBall.Position.x = static_cast<FLOAT>(BALL_AREA.left);
      PlayerBall.Velocity.x *= -1.f;
   }

   if (PlayerBall.Position.x >= BALL_AREA.right)
   {
      PlayerBall.Position.x = static_cast<FLOAT>(BALL_AREA.right - 1);
      PlayerBall.Velocity.x *= -1.f;
   }

   if (PlayerBall.Position.y < BALL_AREA.top)
   {
      PlayerBall.Position.y = static_cast<FLOAT>(BALL_AREA.top);
      PlayerBall.Velocity.y *= -1.f;
   }

   if (PlayerBall.Position.y >= BALL_AREA.bottom)
   {
      PlayerBall.Position.y = static_cast<FLOAT>(BALL_AREA.bottom - 1);
      PlayerBall.Velocity.y *= -1.f;
   }
}


void Game::DrawGameEntities(HDC surfaceContext)
{
   DrawManager::DrawPaddle(surfaceContext, PlayerPaddle);

   for (const auto& brick : Bricks)
      DrawManager::DrawBrick(surfaceContext, brick);

   DrawManager::DrawBall(surfaceContext, PlayerBall.Position);
}
