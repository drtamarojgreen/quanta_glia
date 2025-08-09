#!/bin/bash

# BDD-style test for handling a custom configuration file.

# --- Test Setup ---
TEST_NAME="Test Custom Config"
CONFIG_FILE="tests/bdd/custom_config.yaml"
REPO_NAME="dummy_repo_custom_config"

echo "Running: $TEST_NAME"

# GIVEN: A dummy repository and a custom config file
echo "GIVEN: A dummy repository and a custom config file"
mkdir -p $REPO_NAME
cd $REPO_NAME
git init -b main > /dev/null
echo "This is the readme." > README.md
echo "This is the important file." > IMPORTANT_FILE.txt
git add .
git commit -m "Initial commit" > /dev/null
cd ..

# WHEN: The quanta_glia.py script is run with the custom config
echo "WHEN: The quanta_glia.py script is run with the custom config"
python3 scripts/quanta_glia.py --config $CONFIG_FILE ./$REPO_NAME

# THEN: The knowledge base should only contain files matching the custom config
echo "THEN: The knowledge base should only contain files from the custom config"
KB_PATH="knowledge_base/$REPO_NAME"

if [ -f "$KB_PATH/IMPORTANT_FILE.txt" ] && [ ! -f "$KB_PATH/README.md" ]; then
    echo "SUCCESS: Knowledge base was populated correctly using the custom config."
    # Clean up
    rm -rf $REPO_NAME
    rm -rf $KB_PATH
    # The custom config file is not removed as it's part of the test suite
    exit 0
else
    echo "FAILURE: Knowledge base was not populated correctly."
    # Clean up
    rm -rf $REPO_NAME
    rm -rf $KB_PATH
    exit 1
fi
