#!/bin/sh

OUTPUT_DIR="bin/"

cd "$(dirname "$0")/.."

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

echo "========================================"
echo "Generating Default S4 Image..."
echo "Layout:     generator/layout/example.toml"
echo "Trampoline: generator/trampoline/hello_world.bin"
echo "========================================"



python3 generator/main.py \
    generator/layout/example.toml \
    generator/trampoline/hello_world.bin