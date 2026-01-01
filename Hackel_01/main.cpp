#include <cmath>
#include <raylib.h>

int main() {
  InitWindow(800, 800, "Hackel_01");
  SetTargetFPS(60);
  float t = 0;

  while (!WindowShouldClose()) {
    t += 0.02f;
    BeginDrawing();
    ClearBackground(BLACK);

    // Pure math: nested sin/cos creates flower
    for (int y = 0; y < 800; y++)
      for (int x = 0; x < 800; x++) {
        float dx = x - 400, dy = y - 400;
        float r = sqrt(dx * dx + dy * dy);
        float a = atan2(dy, dx);

        // The magic formula
        if ((int)(r + sin(a * 5 + t) * 30 + sin(r * 0.1 - t) * 20) % 40 < 2)
          DrawPixel(x, y,
                    ColorFromHSV(fmod(a * 50 + r + t * 50, 360), 0.8f, 1.0f));
      }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
