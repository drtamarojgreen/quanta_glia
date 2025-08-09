#!/bin/bash

# ==============================================================================
# QuantaGlia Workplace Test Script
#
# This script automates the manual integration test described in the
# docs/workplace_testing_plan.md.
#
# It performs the following steps:
#   1. Cleans up the 'knowledge_base' directory from previous runs.
#   2. Runs the main 'quanta_glia.py' script, which will use the default
#      test repository ('../quanta_ethos') since no arguments are provided.
#   3. Verifies that the expected output file was created.
#   4. Reports a clear success or failure message.
#
# This test requires a sibling directory named 'quanta_ethos' to exist,
# which can be set up by running the 'scripts/bootstrap.sh' script.
# ==============================================================================

# --- Configuration ---
KNOWLEDGE_BASE_DIR="knowledge_base"
# The file being checked for is 'quanta_ethos.py', which is one of the target files.
EXPECTED_OUTPUT_FILE="knowledge_base/quanta_ethos/quanta_ethos.py"
SCRIPT_TO_TEST="scripts/quanta_glia.py"

# --- Test Logic ---

echo "🚀 Starting QuantaGlia Workplace Integration Test..."

# 1. Cleanup
if [ -d "$KNOWLEDGE_BASE_DIR" ]; then
    echo "🧹 Cleaning up previous test run by removing '$KNOWLEDGE_BASE_DIR'..."
    rm -rf "$KNOWLEDGE_BASE_DIR"
fi

# 2. Run the script
echo "🔬 Running the main script ($SCRIPT_TO_TEST)..."
python3 "$SCRIPT_TO_TEST"

# Capture the exit code of the python script
exit_code=$?
if [ $exit_code -ne 0 ]; then
    echo "❌ TEST FAILED: The script '$SCRIPT_TO_TEST' exited with a non-zero status code: $exit_code."
    exit 1
fi

# 3. Verify the outcome
echo "🔍 Verifying the test outcome..."
if [ -f "$EXPECTED_OUTPUT_FILE" ]; then
    echo "✅ TEST PASSED: The expected output file was created successfully."
    echo "   -> Found: $EXPECTED_OUTPUT_FILE"
    exit 0
else
    echo "❌ TEST FAILED: The expected output file was not found."
    echo "   -> Expected to find: $EXPECTED_OUTPUT_FILE"

    # Provide some debugging help
    if [ -d "$KNOWLEDGE_BASE_DIR" ]; then
        echo "   Debug info: Listing contents of '$KNOWLEDGE_BASE_DIR':"
        ls -lR "$KNOWLEDGE_BASE_DIR"
    else
        echo "   Debug info: The '$KNOWLEDGE_BASE_DIR' directory was not created."
    fi
    exit 1
fi
