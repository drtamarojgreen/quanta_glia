import os
import shutil
import subprocess
from pathlib import Path
import pytest

def test_quanta_glia_bdd():
    # --- Test Setup ---
    TEST_NAME = "Test Quanta Glia BDD"
    print(f"Running: {TEST_NAME}")

    # --- Given: A dummy repository with some key files ---
    print("GIVEN: A dummy repository with key files")
    repo_path = Path("dummy_repo")
    if repo_path.exists():
        shutil.rmtree(repo_path)
    repo_path.mkdir(exist_ok=True)
    
    # Initialize Git repository
    subprocess.run(["git", "init", "-b", "main"], cwd=repo_path, check=True, capture_output=True)
    (repo_path / "README.md").write_text("This is a test README.")
    (repo_path / "LICENSE").write_text("This is a test LICENSE.")
    subprocess.run(["git", "add", "."], cwd=repo_path, check=True, capture_output=True)
    subprocess.run(["git", "commit", "-m", "Initial commit"], cwd=repo_path, check=True, capture_output=True)

    # --- When: The quanta_glia.py script is run on the repository ---
    print("WHEN: The quanta_glia.py script is run on the repository")
    script_path = "scripts/quanta_glia.py"
    subprocess.run(["python3", script_path, str(repo_path)], check=True, capture_output=True)

    # --- Then: The knowledge base should contain the extracted files ---
    print("THEN: The knowledge base should contain the extracted files")
    kb_path = Path("knowledge_base")
    if not kb_path.exists():
        pytest.fail("Knowledge base directory was not created.")
    
    repo_kb_path = kb_path / "dummy_repo"
    if not (repo_kb_path / "README.md").exists() or not (repo_kb_path / "LICENSE").exists():
        pytest.fail("Knowledge base was not populated correctly.")
    else:
        print("SUCCESS: Knowledge base was populated correctly.")

    # --- Clean up ---
    shutil.rmtree(repo_path)
    shutil.rmtree(kb_path)

if __name__ == "__main__":
    # This is a simple way to run the test, for demonstration purposes only
    # In a real test suite, pytest would be used to discover and run the tests
    test_quanta_glia_bdd()
