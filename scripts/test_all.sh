#!/bin/bash

# This script runs all available test suites.

# --- Configuration ---
# Exit immediately if a command exits with a non-zero status.
set -e

# --- BDD Tests ---
echo "--- Running BDD Tests ---"
BDD_TEST_DIR="tests/bdd"

# Find all test files (both .sh and .py) in the BDD test directory
# and make them executable.
find "$BDD_TEST_DIR" -name "test_*.sh" -o -name "test_*.py" -exec chmod +x {} +

# Now, execute them
for test_script in $(find "$BDD_TEST_DIR" -name "test_*.sh" -o -name "test_*.py"); do
    echo ""
    echo "Executing test: $test_script"
    bash "$test_script"
done

echo ""
echo "--- All Tests Completed ---"
exit 0
