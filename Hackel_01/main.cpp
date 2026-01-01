#include <cmath>
#include <cstdio>
#include <raylib.h>

int main() {
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(800, 800, "Hackel_01");
  SetTargetFPS(60);
  float t = 0;

  int frameCount = 0;
  int maxFrames = 180; // 3 seconds at 60fps
  bool recording = true;

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

    // Capture frames
    if (recording && frameCount < maxFrames) {
      char filename[64];
      snprintf(filename, sizeof(filename), "frame_%03d.png", frameCount);
      TakeScreenshot(filename);
      frameCount++;

      if (frameCount >= maxFrames) {
        recording = false;
        TraceLog(LOG_INFO, "Recording complete! Creating GIF...");

        // Auto-create GIF using ffmpeg at 30fps (better for GIF format)
        system("ffmpeg -y -framerate 30 -i frame_%03d.png -vf "
               "palettegen palette.png");
        system("ffmpeg -y -framerate 30 -i frame_%03d.png -i "
               "palette.png -lavfi paletteuse Hackel_01.gif");

        TraceLog(LOG_INFO, "GIF saved as Hackel_01.gif");
        break; // Exit loop
      }
    }
  }

  CloseWindow();
  return 0;
}
