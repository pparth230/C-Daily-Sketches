# C++ Raylib Sketches

A collection of creative coding sketches using C++ and Raylib.

## 📁 Project Structure

```
C++/
├── config.json          # Common build configuration
├── sketches.json        # Registry of all sketches
├── build.sh            # Universal build script
├── sketch.sh           # Sketch manager utility
├── README.md           # This file
├── .gitignore          # Git ignore rules
├── RotatingSquares/    # Individual sketch folder
│   └── main.cpp
└── [YourNewSketch]/    # Add more sketches here
    └── main.cpp
```

## 🚀 Quick Start

### Running a Sketch

From the parent `C++` folder:
```bash
./build.sh RotatingSquares        # Build and run in debug mode
./build.sh RotatingSquares debug  # Explicitly debug mode
./build.sh RotatingSquares release # Build with optimizations
```

Or from within a sketch folder:
```bash
cd RotatingSquares
../build.sh
```

### Creating a New Sketch

**Using the sketch manager (Recommended):**
```bash
./sketch.sh new MyNewSketch
```

This automatically creates:
- ✅ `MyNewSketch/` folder
- ✅ `MyNewSketch/main.cpp` (blank file - write your code here)
- ✅ `MyNewSketch/.vscode/` (IDE configuration)
- ✅ Entry in `sketches.json`

Then just edit `MyNewSketch/main.cpp` and run:
```bash
./sketch.sh run MyNewSketch
```

**Other sketch.sh commands:**
```bash
./sketch.sh list              # List all sketches
./sketch.sh run SketchName    # Build and run a sketch
./sketch.sh info SketchName   # Show sketch information
```

## ⚙️ Configuration

### config.json

This file contains all the common build settings:
- **compiler**: Which compiler to use (clang++, g++, etc.)
- **standard**: C++ standard version (c++17, c++20, etc.)
- **include_paths**: Where to find header files (Raylib)
- **library_paths**: Where to find libraries
- **libraries**: Which libraries to link (raylib)
- **compiler_flags**: Additional compiler warnings/options
- **optimization**: Debug vs Release optimization flags

To modify settings for all sketches, edit `config.json`.

### sketches.json

A registry of all your sketches with metadata:
- **name**: Display name of the sketch
- **description**: What the sketch does
- **folder**: Folder name
- **main_file**: Entry point file (usually main.cpp)
- **status**: working, in-progress, broken, etc.

## 📝 Example Sketch Template

```cpp
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "My Sketch");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Your drawing code here
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## 🔧 Build Modes

- **Debug** (`debug`): Includes debugging symbols, no optimization
- **Release** (`release`): Optimized for performance

## 💡 Tips

1. Each sketch is self-contained in its own folder
2. All sketches share the same build configuration
3. You can override settings per-sketch by creating a local build script
4. The build script automatically finds `.cpp` files if `main.cpp` doesn't exist
5. Use `sketches.json` to keep track of your projects

## 🐛 Troubleshooting

**Build fails with "raylib.h not found"**:
- Check that Raylib is installed: `brew list raylib`
- Verify paths in `config.json` match your Raylib installation

**Permission denied when running build.sh**:
```bash
chmod +x build.sh
```

**Want to use a different compiler?**:
- Edit `config.json` and change the `compiler` field
