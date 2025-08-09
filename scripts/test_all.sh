#!/bin/bash

# ==============================================================================
# QuantaGlia Integration Test Script
#
# This script performs a self-contained integration test of `quanta_glia.py`.
# It verifies that the script can correctly process a local directory,
# extract key files, and store them in the knowledge base.
#
# The test is designed to be runnable without any external dependencies or
# prior workspace setup (like running bootstrap.sh).
#
# It will:
# 1. Create a temporary source directory with a test file.
# 2. Run `quanta_glia.py` against that directory.
# 3. Verify that the output is created as expected.
# 4. Clean up all temporary files and directories.
# 5. Exit with status 0 on success and 1 on failure.
# ==============================================================================

# --- Test Configuration ---
TEST_SOURCE_DIR="test_repo_source"
TEST_FILE_NAME="README.md"
TEST_FILE_CONTENT="This is a test README file for QuantaGlia."
KNOWLEDGE_BASE_DIR="knowledge_base"
EXPECTED_OUTPUT_DIR="${KNOWLEDGE_BASE_DIR}/${TEST_SOURCE_DIR}"
EXPECTED_OUTPUT_FILE="${EXPECTED_OUTPUT_DIR}/${TEST_FILE_NAME}"

# --- Helper Functions ---
cleanup() {
    echo "---"
    echo "🧹 Cleaning up..."
    rm -rf "$TEST_SOURCE_DIR"
    rm -rf "$KNOWLEDGE_BASE_DIR"
    # The script also creates repo_cache, so we clean that too.
    rm -rf "repo_cache"
    echo "Cleanup complete."
}

# --- Main Test Logic ---

# Ensure cleanup happens on script exit, including on failure
trap cleanup EXIT

echo "🚀 Starting QuantaGlia Integration Test..."
echo "---"

# 1. Create test source directory and file
echo "1. Creating temporary test source: '$TEST_SOURCE_DIR'"
mkdir -p "$TEST_SOURCE_DIR"
echo "$TEST_FILE_CONTENT" > "${TEST_SOURCE_DIR}/${TEST_FILE_NAME}"
echo "Test source created."
echo "---"

# 2. Run the quanta_glia.py script
echo "2. Running quanta_glia.py..."
python3 scripts/quanta_glia.py "./${TEST_SOURCE_DIR}"
if [ $? -ne 0 ]; then
    echo "❌ TEST FAILED: quanta_glia.py script exited with a non-zero status."
    exit 1
fi
echo "Script executed."
echo "---"

# 3. Verify the outcome
echo "3. Verifying test output..."
if [ -f "$EXPECTED_OUTPUT_FILE" ]; then
    echo "✅ Verification successful: Output file '$EXPECTED_OUTPUT_FILE' was created."
else
    echo "❌ TEST FAILED: Expected output file '$EXPECTED_OUTPUT_FILE' was not found."
    echo "Contents of knowledge base:"
    ls -R "$KNOWLEDGE_BASE_DIR"
    exit 1
fi

# 4. Verify content
# As an extra check, verify the content matches
if grep -q "$TEST_FILE_CONTENT" "$EXPECTED_OUTPUT_FILE"; then
    echo "✅ Verification successful: File content is correct."
else
    echo "❌ TEST FAILED: Content of '$EXPECTED_OUTPUT_FILE' does not match expected content."
    exit 1
fi

echo "---"
echo "🎉 TEST PASSED"
exit 0
