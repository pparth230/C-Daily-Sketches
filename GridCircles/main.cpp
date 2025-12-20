#include <raylib.h>

void createGridPoints(int gridOffset, int gridSize, int gridSpacing) {
  for (int x = gridOffset; x <= gridOffset + gridSize; x += gridSpacing) {
    for (int y = gridOffset; y <= gridOffset + gridSize; y += gridSpacing) {
      DrawPixel(x, y, WHITE);
    }
  }
}

int main() {
  int canvasWidth = 600;
  int canvasHeight = 600;
  int gridSize = 500;
  int gridSpacing = 25;
  int gridOffset = (canvasHeight - gridSize) / 2;

  InitWindow(canvasWidth, canvasHeight, "Grid Circles");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    createGridPoints(gridOffset, gridSize, gridSpacing);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}