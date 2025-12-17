#include "raylib.h"

void createGridPoints(float rotation) {
  int grid_size = 80;
  int grid_area = 400;
  int offset = (600 - grid_area) / 2;

  for (int x = offset; x <= offset + grid_area; x += grid_size) {
    for (int y = offset; y <= offset + grid_area; y += grid_size) {
      DrawRectanglePro(Rectangle{(float)x, (float)y, 30.0f, 30.0f},
                       Vector2{15.0f, 15.0f}, rotation, WHITE);
    }
  }
}

int main() {
  int canvasWidth = 600;
  int canvasHeight = 600;
  InitWindow(canvasWidth, canvasHeight, "Rotating Squares Grid");
  SetTargetFPS(60);

  float rotation = 0.f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    createGridPoints(rotation);
    rotation += 2.5f;
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
