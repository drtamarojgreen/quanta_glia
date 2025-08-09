#!/bin/bash

# ==============================================================================
# QuantaGlia Bootstrap Script
#
# This script sets up the full development environment by cloning all required
# PrismQuanta repositories as siblings to the current `quanta_glia` directory.
#
# It includes error handling to not exit on failure and logs all output to a
# file for later review.
#
# Usage:
#   1. Clone the `quanta_glia` repository.
#   2. `cd quanta_glia`
#   3. `bash scripts/bootstrap.sh`
# ==============================================================================

# --- Dependency Check ---
# Ensure git is installed before proceeding.
if ! command -v git &> /dev/null; then
    echo "❌ ERROR: git could not be found." | tee -a "$LOG_FILE"
    echo "   Please install git and ensure it is in your PATH." | tee -a "$LOG_FILE"
    exit 1
fi

# --- Configuration ---
# GitHub user who owns the repositories
GH_USER="drtamarojgreen"

# List of repositories to clone
REPOS=(
    "prismquanta"
    "quanta_dorsa"
    "quanta_ethos"
    "quanta_porto"
    "quanta_sensa"
    "quanta_synapse"
    "quanta_tissu"
    "quanta_memora"
    "quanta_serene"
    "quanta_cogno"
    "quanta_retina"
    "quanta_pulsa"
    "quanta_lista"
    "quanta_alarma"
    "quanta_cerebra"
)

# Log file for the script's output
# Use an absolute path to ensure we can write to it after changing directories
LOG_FILE="$(pwd)/bootstrap_setup.log"

# --- Script Logic ---

# Clear previous log file or create a new one
true > "$LOG_FILE"

echo "🚀 Starting QuantaGlia workspace bootstrap..." | tee -a "$LOG_FILE"
echo "Full log will be saved to: $LOG_FILE" | tee -a "$LOG_FILE"

# Navigate to the parent directory (e.g., `workspace/`)
echo "Navigating from '$(pwd)/' to parent directory..." | tee -a "$LOG_FILE"
cd ..
echo "Now in '$(pwd)/'. Preparing to clone sibling repositories." | tee -a "$LOG_FILE"

echo "Cloning required repositories from GitHub..." | tee -a "$LOG_FILE"
echo "--------------------------------------------------" | tee -a "$LOG_FILE"

CLONE_SUCCESS_COUNT=0
CLONE_FAIL_COUNT=0

for repo in "${REPOS[@]}"; do
    # Check if the directory already exists
    if [ -d "$repo" ]; then
        echo "✅ Repository '$repo' already exists. Skipping clone." | tee -a "$LOG_FILE"
    else
        echo "Cloning '$repo'..." | tee -a "$LOG_FILE"
        # Attempt to clone the repository.
        # stdout and stderr from git are appended directly to the log file for detailed error info.
        if git clone "https://github.com/${GH_USER}/${repo}.git" >> "$LOG_FILE" 2>&1; then
            echo "✅ Successfully cloned '$repo'." | tee -a "$LOG_FILE"
            ((CLONE_SUCCESS_COUNT++))
        else
            echo "❌ ERROR: Failed to clone '$repo'." | tee -a "$LOG_FILE"
            echo "   Please check the repository name, permissions, and your network connection." | tee -a "$LOG_FILE"
            echo "   Check '$LOG_FILE' for more details." | tee -a "$LOG_FILE"
            ((CLONE_FAIL_COUNT++))
        fi
    fi
    echo "--------------------------------------------------" | tee -a "$LOG_FILE"
done

echo "🎉 Bootstrap complete!" | tee -a "$LOG_FILE"
echo "Summary:" | tee -a "$LOG_FILE"
echo "  - Successfully cloned: $CLONE_SUCCESS_COUNT" | tee -a "$LOG_FILE"
echo "  - Failed to clone:     $CLONE_FAIL_COUNT" | tee -a "$LOG_FILE"

if [ "$CLONE_FAIL_COUNT" -gt 0 ]; then
    echo "⚠️ Some repositories failed to clone. Please review the log above and in '$LOG_FILE'." | tee -a "$LOG_FILE"
fi

echo "Final workspace directory structure in '$(pwd)/':" | tee -a "$LOG_FILE"
ls -d ./*/ | tee -a "$LOG_FILE"
