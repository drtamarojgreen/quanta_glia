#!/bin/bash

# BDD-style test for handling repositories with no target files.

# --- Test Setup ---
TEST_NAME="Test No Target Files"
echo "Running: $TEST_NAME"

# GIVEN: A dummy repository with files that don't match target_topics
echo "GIVEN: A dummy repository with no target files"
mkdir -p dummy_repo_no_targets
cd dummy_repo_no_targets
git init -b main > /dev/null
echo "This is a test file." > some_other_file.txt
git add .
git commit -m "Initial commit" > /dev/null
cd ..

# WHEN: The quanta_glia.py script is run on the repository
echo "WHEN: The quanta_glia.py script is run on the repository"
# Note: The script is run from the root, so the path is correct.
python3 scripts/quanta_glia.py ./dummy_repo_no_targets

# THEN: The knowledge base should not contain a directory for this repo
echo "THEN: The knowledge base should not contain a directory for this repo"
# The script should not create a directory if no files are extracted.
if [ ! -d "knowledge_base/dummy_repo_no_targets" ]; then
    echo "SUCCESS: Knowledge base directory was not created, as expected."
    # Clean up
    rm -rf dummy_repo_no_targets
    exit 0
else
    echo "FAILURE: Knowledge base directory was created unexpectedly."
    # Clean up
    rm -rf dummy_repo_no_targets
    rm -rf knowledge_base/dummy_repo_no_targets
    exit 1
fi
