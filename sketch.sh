#!/bin/bash

# Sketch management utility
# Usage: ./sketch.sh [command] [args]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SKETCHES_FILE="$SCRIPT_DIR/sketches.json"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

show_help() {
    echo "🎨 C++ Raylib Sketch Manager"
    echo ""
    echo "Usage: ./sketch.sh [command] [args]"
    echo ""
    echo "Commands:"
    echo "  list                  - List all sketches"
    echo "  new <name>           - Create a new sketch"
    echo "  run <name> [mode]    - Build and run a sketch (mode: debug/release)"
    echo "  info <name>          - Show sketch information"
    echo "  help                 - Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./sketch.sh list"
    echo "  ./sketch.sh new MyAwesomeSketch"
    echo "  ./sketch.sh run RotatingSquares"
    echo "  ./sketch.sh run RotatingSquares release"
}

list_sketches() {
    echo -e "${BLUE}📚 Available Sketches:${NC}"
    echo ""
    
    python3 << EOF
import json

with open('$SKETCHES_FILE', 'r') as f:
    data = json.load(f)
    
for i, sketch in enumerate(data['sketches'], 1):
    status_emoji = {
        'working': '✅',
        'in-progress': '🚧',
        'broken': '❌',
        'experimental': '🧪'
    }.get(sketch.get('status', 'unknown'), '❓')
    
    print(f"{i}. {status_emoji} {sketch['name']}")
    print(f"   📁 {sketch['folder']}")
    print(f"   📝 {sketch['description']}")
    print()
EOF
}

create_sketch() {
    if [ -z "$1" ]; then
        echo -e "${RED}❌ Error: Please provide a sketch name${NC}"
        echo "Usage: ./sketch.sh new <name>"
        exit 1
    fi
    
    SKETCH_NAME="$1"
    SKETCH_FOLDER="$SCRIPT_DIR/$SKETCH_NAME"
    
    if [ -d "$SKETCH_FOLDER" ]; then
        echo -e "${RED}❌ Error: Sketch folder already exists: $SKETCH_NAME${NC}"
        exit 1
    fi
    
    echo -e "${BLUE}🎨 Creating new sketch: $SKETCH_NAME${NC}"
    
    # Create folder
    mkdir -p "$SKETCH_FOLDER"
    
    # Create blank main.cpp
    touch "$SKETCH_FOLDER/main.cpp"

    
    # Create .vscode folder and configuration files
    mkdir -p "$SKETCH_FOLDER/.vscode"
    
    # Create c_cpp_properties.json for IntelliSense
    cat > "$SKETCH_FOLDER/.vscode/c_cpp_properties.json" << 'VSCODE_CONFIG'
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**",
                "/opt/homebrew/include"
            ],
            "defines": [],
            "macFrameworkPath": [
                "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-arm64"
        }
    ],
    "version": 4
}
VSCODE_CONFIG
    
    # Create compile_commands.json
    cat > "$SKETCH_FOLDER/compile_commands.json" << COMPILE_COMMANDS
[
  {
    "directory": "$SKETCH_FOLDER",
    "command": "clang++ -std=c++17 -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib main.cpp -o main",
    "file": "main.cpp"
  }
]
COMPILE_COMMANDS
    
    # Add to sketches.json
    python3 << EOF
import json

with open('$SKETCHES_FILE', 'r') as f:
    data = json.load(f)

new_sketch = {
    "name": "$SKETCH_NAME",
    "description": "New sketch - add description here",
    "folder": "$SKETCH_NAME",
    "main_file": "main.cpp",
    "status": "in-progress"
}

data['sketches'].append(new_sketch)

with open('$SKETCHES_FILE', 'w') as f:
    json.dump(data, f, indent=2)
EOF
    
    echo -e "${GREEN}✅ Sketch created successfully!${NC}"
    echo ""
    echo "📁 Location: $SKETCH_FOLDER"
    echo "📝 Edit: $SKETCH_FOLDER/main.cpp"
    echo ""
    echo "To run your sketch:"
    echo "  ./sketch.sh run $SKETCH_NAME"
    echo "  or"
    echo "  ./build.sh $SKETCH_NAME"
}

run_sketch() {
    if [ -z "$1" ]; then
        echo -e "${RED}❌ Error: Please provide a sketch name${NC}"
        echo "Usage: ./sketch.sh run <name> [mode]"
        exit 1
    fi
    
    SKETCH_NAME="$1"
    BUILD_MODE="${2:-debug}"
    
    if [ ! -d "$SCRIPT_DIR/$SKETCH_NAME" ]; then
        echo -e "${RED}❌ Error: Sketch not found: $SKETCH_NAME${NC}"
        echo ""
        echo "Available sketches:"
        list_sketches
        exit 1
    fi
    
    echo -e "${BLUE}🚀 Running sketch: $SKETCH_NAME (mode: $BUILD_MODE)${NC}"
    echo ""
    
    "$SCRIPT_DIR/build.sh" "$SKETCH_NAME" "$BUILD_MODE"
}

show_info() {
    if [ -z "$1" ]; then
        echo -e "${RED}❌ Error: Please provide a sketch name${NC}"
        exit 1
    fi
    
    python3 << EOF
import json

with open('$SKETCHES_FILE', 'r') as f:
    data = json.load(f)

sketch = next((s for s in data['sketches'] if s['name'] == '$1'), None)

if sketch:
    print(f"📊 Sketch Information")
    print(f"")
    print(f"Name:        {sketch['name']}")
    print(f"Description: {sketch['description']}")
    print(f"Folder:      {sketch['folder']}")
    print(f"Main File:   {sketch['main_file']}")
    print(f"Status:      {sketch['status']}")
else:
    print("❌ Sketch not found: $1")
EOF
}

# Main command dispatcher
case "${1:-help}" in
    list|ls)
        list_sketches
        ;;
    new|create)
        create_sketch "$2"
        ;;
    run)
        run_sketch "$2" "$3"
        ;;
    info)
        show_info "$2"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        echo -e "${RED}❌ Unknown command: $1${NC}"
        echo ""
        show_help
        exit 1
        ;;
esac
