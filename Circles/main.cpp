#include "raylib.h"
#include <cmath>

void concentricCircle(float canvasWidth, float canvasHeight, float radius) {
  float thickness = 1.0f;
  for (int i = 0; i < 15; i++) {
    DrawRing(Vector2{(canvasWidth / 2), (canvasHeight / 2)},
             radius - thickness / 2, radius + thickness / 2, 0, 360, 36, WHITE);
    radius += 20;
    float wave = fmod(GetTime() * 2.0f, 2.0f);
    if (wave > 1.0f)
      wave = 2.0f - wave;
    float offset = abs(i - wave * 14);
    thickness = 11.0f - offset;
    if (thickness < 1.0f)
      thickness = 1.0f;
  }
}

int main() {
  float canvasWidth = 800;
  float canvasHeight = 800;
  float radius = 20;
  InitWindow(canvasWidth, canvasHeight, "Circles");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    concentricCircle(canvasWidth, canvasHeight, radius);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}