#!/bin/bash

# ==============================================================================
# QuantaGlia Master Test Script
#
# This script is the main entry point for all tests. It orchestrates the setup
# and execution of the entire test suite for the QuantaGlia workspace.
#
# It performs the following steps:
#   1. Sets up the workspace by cloning all required repositories using bootstrap.sh.
#   2. Runs a self-contained integration test for the main script.
#   3. Runs a specific integration test against the 'quanta_ethos' repo.
#   4. Runs the test suites for all repositories in the workspace.
#
# The script will exit immediately if any command fails.
# ==============================================================================

set -e # Exit immediately if a command exits with a non-zero status.

# --- Helper to print section headers ---
print_header() {
    echo ""
    echo "=============================================================================="
    echo "  $1"
    echo "=============================================================================="
    echo ""
}

# --- Main Test Execution ---

print_header "STEP 1: BOOTSTRAP - Setting up the workspace"
bash scripts/bootstrap.sh

print_header "STEP 2: UNIT TESTS"
echo "🔬 Running unit tests for quanta_glia..."
python3 -m unittest discover tests/unit/

print_header "STEP 3: SELF-CONTAINED INTEGRATION TEST"
bash scripts/test_integration.sh

print_header "STEP 4: WORKPLACE INTEGRATION TEST (against quanta_ethos)"

# This section is adapted from the original workplace_test_all.sh
# --- Configuration for Workplace Test ---
KNOWLEDGE_BASE_DIR="knowledge_base"
EXPECTED_OUTPUT_FILE="knowledge_base/quanta_ethos/quanta_ethos.py"
SCRIPT_TO_TEST="scripts/quanta_glia.py"

# 1. Cleanup from previous runs
echo "🧹 Cleaning up previous test run by removing '$KNOWLEDGE_BASE_DIR'..."
rm -rf "$KNOWLEDGE_BASE_DIR"

# 2. Run the main script
echo "🔬 Running the main script ($SCRIPT_TO_TEST) for workplace test..."
python3 "$SCRIPT_TO_TEST"

# 3. Verify the outcome
echo "🔍 Verifying the workplace test outcome..."
if [ -f "$EXPECTED_OUTPUT_FILE" ]; then
    echo "✅ Workplace test PASSED: The expected output file was created successfully."
    echo "   -> Found: $EXPECTED_OUTPUT_FILE"
else
    echo "❌ Workplace test FAILED: The expected output file was not found."
    echo "   -> Expected to find: $EXPECTED_OUTPUT_FILE"

    # Provide some debugging help
    if [ -d "$KNOWLEDGE_BASE_DIR" ]; then
        echo "   Debug info: Listing contents of '$KNOWLEDGE_BASE_DIR':"
        ls -lR "$KNOWLEDGE_BASE_DIR"
    else
        echo "   Debug info: The '$KNOWLEDGE_BASE_DIR' directory was not created."
    fi
    exit 1 # Explicitly exit with failure
fi

print_header "STEP 5: WORKSPACE-WIDE TESTS (running tests in all repos)"
bash scripts/test_workspace.sh

print_header "🎉 ALL TESTS COMPLETED SUCCESSFULLY 🎉"
echo "The entire QuantaGlia workspace test suite has passed."
