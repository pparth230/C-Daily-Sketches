#!/bin/bash

# Universal build script for C++ Raylib sketches
# Usage: ./build.sh [sketch_folder] [mode]
# Example: ./build.sh RotatingSquares debug
# Example: ./build.sh RotatingSquares release
# If run from within a sketch folder: ./build.sh

set -e

# Determine the script's directory (parent C++ folder)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_FILE="$SCRIPT_DIR/config.json"

# Check if config.json exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo "❌ Error: config.json not found in $SCRIPT_DIR"
    exit 1
fi

# Parse arguments
if [ $# -eq 0 ]; then
    # If no arguments, assume we're in a sketch folder
    SKETCH_FOLDER="$(pwd)"
    BUILD_MODE="debug"
elif [ $# -eq 1 ]; then
    SKETCH_FOLDER="$SCRIPT_DIR/$1"
    BUILD_MODE="debug"
else
    SKETCH_FOLDER="$SCRIPT_DIR/$1"
    BUILD_MODE="$2"
fi

# Validate sketch folder
if [ ! -d "$SKETCH_FOLDER" ]; then
    echo "❌ Error: Sketch folder not found: $SKETCH_FOLDER"
    exit 1
fi

# Change to sketch directory
cd "$SKETCH_FOLDER"
echo "📁 Building in: $SKETCH_FOLDER"
echo "🔧 Build mode: $BUILD_MODE"

# Read configuration using Python (available on macOS by default)
read_config() {
    python3 -c "import json; config=json.load(open('$CONFIG_FILE')); print(config$1)" 2>/dev/null || echo ""
}

# Extract configuration values
COMPILER=$(python3 -c "import json; print(json.load(open('$CONFIG_FILE'))['compiler'])")
STANDARD=$(python3 -c "import json; print(json.load(open('$CONFIG_FILE'))['standard'])")
OUTPUT=$(python3 -c "import json; print(json.load(open('$CONFIG_FILE'))['output_name'])")

# Build include paths
INCLUDE_PATHS=$(python3 -c "import json; paths=json.load(open('$CONFIG_FILE'))['include_paths']; print(' '.join([f'-I{p}' for p in paths]))")

# Build library paths
LIBRARY_PATHS=$(python3 -c "import json; paths=json.load(open('$CONFIG_FILE'))['library_paths']; print(' '.join([f'-L{p}' for p in paths]))")

# Build libraries
LIBRARIES=$(python3 -c "import json; libs=json.load(open('$CONFIG_FILE'))['libraries']; print(' '.join([f'-l{lib}' for lib in libs]))")

# Build compiler flags
COMPILER_FLAGS=$(python3 -c "import json; flags=json.load(open('$CONFIG_FILE'))['compiler_flags']; print(' '.join(flags))")

# Get optimization flag based on mode
if [ "$BUILD_MODE" = "release" ]; then
    OPT_FLAG=$(python3 -c "import json; print(json.load(open('$CONFIG_FILE'))['optimization']['release'])")
else
    OPT_FLAG=$(python3 -c "import json; print(json.load(open('$CONFIG_FILE'))['optimization']['debug'])")
fi

# Find the main.cpp file (or first .cpp file)
MAIN_FILE="main.cpp"
if [ ! -f "$MAIN_FILE" ]; then
    MAIN_FILE=$(find . -maxdepth 1 -name "*.cpp" | head -n 1)
    if [ -z "$MAIN_FILE" ]; then
        echo "❌ Error: No .cpp file found in $SKETCH_FOLDER"
        exit 1
    fi
fi

echo "📝 Source file: $MAIN_FILE"

# Build the compile command
COMPILE_CMD="$COMPILER $MAIN_FILE -o $OUTPUT -std=$STANDARD $INCLUDE_PATHS $LIBRARY_PATHS $LIBRARIES $COMPILER_FLAGS $OPT_FLAG"

echo "🔨 Compiling..."
echo "   Command: $COMPILE_CMD"
echo ""

# Compile
eval $COMPILE_CMD

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    echo "🚀 Running $OUTPUT..."
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    ./$OUTPUT
else
    echo "❌ Build failed!"
    exit 1
fi
