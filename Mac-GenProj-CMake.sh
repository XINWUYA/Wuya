#!/bin/bash

# Generate CMake project files for macOS

echo "========================================"
echo "Wuya CMake Project Generator (macOS)"
echo "========================================"
echo ""

# Create build directory if it doesn't exist
mkdir -p build

# Default generator
GENERATOR="Xcode"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        xcode)
            GENERATOR="Xcode"
            shift
            ;;
        ninja)
            GENERATOR="Ninja"
            shift
            ;;
        make)
            GENERATOR="Unix Makefiles"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [xcode|ninja|make]"
            exit 1
            ;;
    esac
done

echo "Using generator: $GENERATOR"
echo ""

# Configure CMake
cd build
cmake -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Debug ..

if [ $? -ne 0 ]; then
    echo ""
    echo "========================================"
    echo "CMake configuration failed!"
    echo "========================================"
    cd ..
    exit 1
fi

echo ""
echo "========================================"
echo "CMake project files generated successfully!"
echo "Build directory: build"
echo "========================================"
echo ""
echo "Next steps:"
echo "  1. Open build/Wuya.xcodeproj (if using Xcode)"
echo "  2. Build the project in your IDE"
echo "  Or use: cmake --build build --config Debug"
echo ""

cd ..
