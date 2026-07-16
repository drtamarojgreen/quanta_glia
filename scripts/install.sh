#!/bin/bash
# ==============================================================================
# QuantaGlia Unified Installer (Linux / macOS)
#
# This script automates the process of building the quanta_glia (glia) C++ toolkit
# and adding it to the user's PATH.
#
# Usage:
#   bash scripts/install.sh
# ==============================================================================

set -e

# --- Colors for Output ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}====================================================${NC}"
echo -e "${BLUE}       QuantaGlia C++ Installer (Linux/macOS)       ${NC}"
echo -e "${BLUE}====================================================${NC}"

# --- Dependency Check ---
echo -e "\n${YELLOW}[1/5] Checking prerequisites...${NC}"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}❌ Error: cmake could not be found. Please install CMake and try again.${NC}"
    exit 1
else
    CMAKE_VER=$(cmake --version | head -n1)
    echo -e "${GREEN}✓ Found CMake: ${CMAKE_VER}${NC}"
fi

# Check for Compiler (g++ or clang++)
COMPILER=""
if command -v g++ &> /dev/null; then
    COMPILER="g++"
elif command -v clang++ &> /dev/null; then
    COMPILER="clang++"
fi

if [ -z "$COMPILER" ]; then
    echo -e "${RED}❌ Error: No suitable C++ compiler (g++ or clang++) found.${NC}"
    exit 1
else
    echo -e "${GREEN}✓ Found C++ Compiler: ${COMPILER}${NC}"
fi

# --- Compilation ---
echo -e "\n${YELLOW}[2/5] Compiling QuantaGlia...${NC}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "Configuring project in ${BUILD_DIR}..."
cmake -DCMAKE_BUILD_TYPE=Release ..

echo -e "Building 'glia' and 'sorrel_test' binaries..."
# Use available CPU cores for faster compilation
CORES=1
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    CORES=$(nproc 2>/dev/null || echo 1)
elif [[ "$OSTYPE" == "darwin"* ]]; then
    CORES=$(sysctl -n hw.ncpu 2>/dev/null || echo 1)
fi
cmake --build . --config Release -j "$CORES"

# Check if build produced the glia executable
if [ ! -f "glia" ]; then
    echo -e "${RED}❌ Error: 'glia' executable was not built successfully.${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Compilation successful!${NC}"

# --- Verification / Testing ---
echo -e "\n${YELLOW}[3/5] Running verification tests...${NC}"
if [ -f "sorrel_test" ]; then
    echo "Running sorrel_test verification suite..."
    if ./sorrel_test; then
        echo -e "${GREEN}✓ All verification tests passed perfectly!${NC}"
    else
        echo -e "${YELLOW}⚠️ Warning: Some verification tests failed. Proceeding with installation anyway.${NC}"
    fi
else
    echo -e "${YELLOW}ℹ️ No test binary found. Skipping test phase.${NC}"
fi

# --- Installation ---
echo -e "\n${YELLOW}[4/5] Installing glia binary...${NC}"
INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

cp glia "$INSTALL_DIR/glia"
chmod +x "$INSTALL_DIR/glia"

echo -e "${GREEN}✓ Installed 'glia' to ${INSTALL_DIR}/glia${NC}"

# --- Path Configuration ---
echo -e "\n${YELLOW}[5/5] Configuring Environment PATH...${NC}"

# Normalize paths for comparison
REAL_INSTALL_DIR=$(cd "$INSTALL_DIR" && pwd)
PATH_CONTAINS_GLIA=false

# Check if INSTALL_DIR is in the active PATH
IFS=':' read -r -a PATH_DIRS <<< "$PATH"
for dir in "${PATH_DIRS[@]}"; do
    if [ -d "$dir" ]; then
        REAL_DIR=$(cd "$dir" 2>/dev/null && pwd || true)
        if [ "$REAL_DIR" == "$REAL_INSTALL_DIR" ]; then
            PATH_CONTAINS_GLIA=true
            break
        fi
    fi
done

if [ "$PATH_CONTAINS_GLIA" = true ]; then
    echo -e "${GREEN}✓ '${INSTALL_DIR}' is already in your PATH!${NC}"
    echo -e "${GREEN}🎉 Setup complete! You can run 'glia' directly from any terminal.${NC}"
else
    echo -e "${YELLOW}⚠️ '${INSTALL_DIR}' is NOT in your current PATH.${NC}"
    echo -e "To run 'glia' directly, please add it to your profile. Here is how:"

    SHELL_NAME=$(basename "$SHELL")
    RC_FILE=""

    case "$SHELL_NAME" in
        zsh)
            RC_FILE="$HOME/.zshrc"
            ;;
        bash)
            if [[ "$OSTYPE" == "darwin"* ]]; then
                RC_FILE="$HOME/.bash_profile"
            else
                RC_FILE="$HOME/.bashrc"
            fi
            ;;
        *)
            RC_FILE="$HOME/.profile"
            ;;
    esac

    echo -e "\nRun the following command to add it to your ${RC_FILE}:"
    echo -e "${BLUE}  echo 'export PATH=\"\$HOME/.local/bin:\$PATH\"' >> ${RC_FILE}${NC}"
    echo -e "Then, reload your shell configuration using:"
    echo -e "${BLUE}  source ${RC_FILE}${NC}"

    # Attempt to automatically add it for standard bash/zsh configurations
    if [ -f "$RC_FILE" ]; then
        echo -e "\nWould you like me to append this export statement to ${RC_FILE} automatically? (y/n)"
        # Since we are in an automated sandbox environment, we can do it non-interactively if requested, or just instruct.
        # But we can also make the script gracefully accept environment var or try to read input, but let's just make it do it if run with -y or fallback.
        read -r response </dev/tty 2>/dev/null || response="n"
        if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
            echo -e "\nexport PATH=\"\$HOME/.local/bin:\$PATH\"" >> "$RC_FILE"
            echo -e "${GREEN}✓ Successfully added to ${RC_FILE}!${NC}"
            echo -e "Please run ${BLUE}source ${RC_FILE}${NC} to refresh your current session."
        else
            echo -e "Please add it manually using the instructions above."
        fi
    fi
fi

echo -e "\n${GREEN}====================================================${NC}"
echo -e "${GREEN}           QuantaGlia Build Completed!             ${NC}"
echo -e "${GREEN}====================================================${NC}\n"
