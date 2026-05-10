#!/bin/bash

# quanta_lista/test.sh
# Structural verification script for Quanta Lista

EXIT_CODE=0

echo "Running structural verification for Quanta Lista..."

# 1. Check for mandatory files
MANDATORY_FILES=(
    "DESIGN.md"
    "RESTRICTIONS.md"
    "CMakeLists.txt"
    "src/main.cpp"
    "src/task.h"
    "src/task_manager.h"
    "src/task_manager.cpp"
    "src/config_manager.h"
    "src/config_manager.cpp"
    "src/mainwindow.h"
    "src/mainwindow.cpp"
)

for file in "${MANDATORY_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Missing mandatory file: $file"
        EXIT_CODE=1
    else
        echo "✅ Found: $file"
    fi
done

# 2. Enforce Restrictions
echo "Checking for prohibited patterns..."

# Check for catch blocks that don't contain a semi-colon
# Using perl-compatible regex for multiline matching
if grep -Pzo "(?s)catch\s*\(.*?\)\s*\{[^;]*?\}" src/*; then
    echo "❌ Violation: Potentially empty or non-functional catch block found."
    EXIT_CODE=1
fi

if [ $EXIT_CODE -eq 0 ]; then
    echo "🎉 All structural checks passed!"
else
    echo "❌ Structural verification failed."
fi

exit $EXIT_CODE
