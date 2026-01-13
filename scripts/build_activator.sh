#!/bin/sh
#
# Build script for S4 Activator (In-Tree Build)
#

set -e

# Project root
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ACTIVATOR_DIR="${PROJECT_ROOT}/freebsd-src/stand/efi/s4_activator"

# Check FreeBSD source tree
if [ ! -d "${PROJECT_ROOT}/freebsd-src/stand" ]; then
    echo "Error: FreeBSD source tree not found"
    echo "Run: git submodule update --init --depth 1"
    exit 1
fi

# Check if running on FreeBSD
if [ "$(uname)" != "FreeBSD" ]; then
    echo "Error: This script must be run on FreeBSD"
    exit 1
fi

# Build s4_activator
echo "Building S4 Activator..."
cd "${ACTIVATOR_DIR}"
make

echo ""
echo "Build complete!"
echo "Output: ${ACTIVATOR_DIR}/s4_activator.efi"
