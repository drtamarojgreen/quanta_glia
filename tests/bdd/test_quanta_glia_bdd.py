#!/bin/bash

# BDD-style test for quanta_glia.py

# --- Test Setup ---
TEST_NAME="Test Quanta Glia BDD"
echo "Running: $TEST_NAME"

# Given: A dummy repository with some key files
echo "GIVEN: A dummy repository with key files"
mkdir -p dummy_repo
cd dummy_repo
git init -b main
echo "This is a test README." > README.md
echo "This is a test LICENSE." > LICENSE
git add .
git commit -m "Initial commit"
cd ..

# When: The quanta_glia.py script is run on the repository
echo "WHEN: The quanta_glia.py script is run on the repository"
python3 scripts/quanta_glia.py ./dummy_repo

# Then: The knowledge base should contain the extracted files
echo "THEN: The knowledge base should contain the extracted files"
if [ -f "knowledge_base/dummy_repo/README.md" ] && [ -f "knowledge_base/dummy_repo/LICENSE" ]; then
    echo "SUCCESS: Knowledge base was populated correctly."
    # Clean up
    rm -rf dummy_repo
    rm -rf knowledge_base/dummy_repo
    exit 0
else
    echo "FAILURE: Knowledge base was not populated correctly."
    # Clean up
    rm -rf dummy_repo
    rm -rf knowledge_base/dummy_repo
    exit 1
fi
