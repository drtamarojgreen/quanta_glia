#!/bin/bash

# BDD-style test for the MAX_REPOS functionality.

# --- Test Setup ---
TEST_NAME="Test MAX_REPOS Limit"
# The default in config.yaml is 10
MAX_REPOS=10
# Create one more than the max to test the limit
NUM_REPOS_TO_CREATE=$(($MAX_REPOS + 1))
REPO_PREFIX="dummy_repo_max_test_"

echo "Running: $TEST_NAME"

# GIVEN: More dummy repositories than the MAX_REPOS limit
echo "GIVEN: $NUM_REPOS_TO_CREATE dummy repositories"
repo_list=""
for i in $(seq 1 $NUM_REPOS_TO_CREATE); do
    repo_name="${REPO_PREFIX}${i}"
    mkdir -p $repo_name
    cd $repo_name
    git init -b main > /dev/null
    echo "readme" > README.md
    git add .
    git commit -m "Initial commit" > /dev/null
    cd ..
    # Add the relative path to our list
    repo_list="$repo_list ./$repo_name"
done

# WHEN: The quanta_glia.py script is run with all repositories
echo "WHEN: The quanta_glia.py script is run with $NUM_REPOS_TO_CREATE repositories"
python3 scripts/quanta_glia.py $repo_list

# THEN: The knowledge base should only contain MAX_REPOS directories
echo "THEN: The knowledge base should only contain $MAX_REPOS directories"
# Count the number of directories created in the knowledge base
# We grep for the prefix to ensure we're only counting our test repos
num_kb_dirs=$(ls -d1 knowledge_base/${REPO_PREFIX}*/ | wc -l)

if [ "$num_kb_dirs" -eq "$MAX_REPOS" ]; then
    echo "SUCCESS: Knowledge base contains exactly $MAX_REPOS directories."
    # Clean up
    rm -rf ${REPO_PREFIX}*
    rm -rf knowledge_base/${REPO_PREFIX}*
    exit 0
else
    echo "FAILURE: Knowledge base contains $num_kb_dirs directories, but expected $MAX_REPOS."
    # Clean up
    rm -rf ${REPO_PREFIX}*
    rm -rf knowledge_base/${REPO_PREFIX}*
    exit 1
fi
