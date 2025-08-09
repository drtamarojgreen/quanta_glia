#!/bin/bash

# test_workspace.sh
# Purpose: Run tests for all repositories in the PrismQuanta workspace.
# This script should be run from the quanta_glia directory as per the README.

set -e # Exit immediately if a command exits with a non-zero status.

# --- Configuration ---
# Repositories to test, as listed in the README.
# The script assumes it's in quanta_glia and these are sibling directories.
REPOS=(
    "quanta_glia"
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

# --- Colors for output ---
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# --- Main Logic ---
echo -e "${YELLOW}Starting PrismQuanta workspace test suite...${NC}"
echo "This script will iterate through all known repositories and run their respective tests."
echo "---------------------------------------------------------------------"

# The script is in quanta_glia, so we need to go to the parent directory
WORKSPACE_DIR="$(pwd)/.."
cd "$WORKSPACE_DIR"

FAILED_REPOS=()
SUCCESS_COUNT=0

for repo in "${REPOS[@]}"; do
    echo -e "\n${YELLOW}>>> Checking repository: $repo${NC}"

    if [ ! -d "$repo" ]; then
        echo -e "${RED}Error: Repository '$repo' not found. Did you run bootstrap.sh?${NC}"
        FAILED_REPOS+=("$repo (Not Found)")
        continue
    fi

    cd "$repo"

    # Heuristic to find and run tests
    TEST_COMMAND=""
    if [ -f "test.sh" ]; then
        TEST_COMMAND="./test.sh"
    elif [ -f "pytest.ini" ] || [ -d "tests" ] || [ -d "test" ]; then
        TEST_COMMAND="pytest"
    elif [ -f "Makefile" ] && grep -q -E "^test:" "Makefile"; then
        TEST_COMMAND="make test"
    else
        echo -e "${YELLOW}Warning: No standard test script found for '$repo'. Skipping.${NC}"
        cd ..
        continue
    fi

    echo "Found test command: '$TEST_COMMAND'. Executing..."
    
    if ! $TEST_COMMAND; then
        echo -e "${RED}Error: Tests failed in repository '$repo'.${NC}"
        FAILED_REPOS+=("$repo")
    else
        echo -e "${GREEN}Success: All tests passed in '$repo'.${NC}"
        ((SUCCESS_COUNT++))
    fi

    cd .. # Return to the workspace root for the next iteration
done

# --- Summary ---
echo -e "\n---------------------------------------------------------------------"
echo -e "${YELLOW}Test Suite Summary:${NC}"
echo -e "---------------------------------------------------------------------"

echo "Total repositories checked: ${#REPOS[@]}"
echo -e "${GREEN}Successful repositories: $SUCCESS_COUNT${NC}"

if [ ${#FAILED_REPOS[@]} -ne 0 ]; then
    echo -e "${RED}Failed repositories (${#FAILED_REPOS[@]}):${NC}"
    for failed_repo in "${FAILED_REPOS[@]}"; do
        echo -e "  - ${RED}$failed_repo${NC}"
    done
    echo -e "\n${RED}Workspace tests finished with errors.${NC}"
    exit 1
else
    echo -e "\n${GREEN}All repository tests passed successfully! Workspace is healthy.${NC}"
    exit 0
fi