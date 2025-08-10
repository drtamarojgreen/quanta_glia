# BDD Testing Plan for QuantaGlia

This document outlines the strategy for Behavior-Driven Development (BDD) testing within the QuantaGlia project. It covers the current approach, proposes a more robust future-state framework, and provides actionable examples for implementation.

---

## 1. Introduction to BDD

Behavior-Driven Development (BDD) is a software development methodology that encourages collaboration between developers, quality assurance (QA) professionals, and business stakeholders. It aims to create a shared understanding of how an application should behave by using a natural, human-readable language called Gherkin.

The core benefits of BDD include:
- **Clarity:** Scenarios are written in plain language, making them accessible to everyone.
- **Collaboration:** It bridges the gap between technical and non-technical team members.
- **Focus on Business Value:** Tests are framed around user-centric behaviors and outcomes.

---

## 2. Current BDD-Style Testing

The project currently employs a BDD-style test located at `tests/bdd/test_quanta_glia_bdd.py`.

- **Format:** This test is a shell script, not a Python file.
- **Method:** It uses `echo` statements with `GIVEN`, `WHEN`, and `THEN` prefixes to describe the test flow in a BDD-like manner.
- **Functionality:** It performs a simple integration test on the core `scripts/quanta_glia.py` script by:
    1.  **Given** a new dummy Git repository.
    2.  **When** the `quanta_glia.py` script is run on it.
    3.  **Then** it asserts that the script correctly extracts key files into the `knowledge_base` directory.

This script serves as an excellent, lightweight proof-of-concept for BDD. However, as the QuantaGlia project grows, a more structured and scalable framework is recommended.

---

## 3. Proposed BDD Framework: Native Python with `pytest`

To enhance our BDD capabilities while adhering to a native Python environment, this plan proposes the adoption of **`pytest`**, the de-facto standard for testing in Python. We can achieve the goals of BDD by using `pytest` conventions and features, without needing a separate Gherkin parser like `behave`.

**Why `pytest` for BDD?**
- **Truly Native:** It's pure Python. Test logic and descriptions live in the same `.py` files.
- **Powerful Fixtures:** `pytest` fixtures are a clean, modular way to manage the `Given` (setup) part of any test.
- **Clear Assertions:** `pytest` uses the standard `assert` statement and provides detailed feedback on failures.
- **No Extra Layers:** Avoids the need to maintain separate `.feature` files and the "glue code" that connects them to Python.
- **Huge Ecosystem:** Fully compatible with the vast ecosystem of `pytest` plugins.

### 3.1. Proposed Directory Structure

We will use the standard `pytest` discovery structure. BDD-style tests will be grouped in their own directory for clarity.

```text
tests/
└── bdd/
    ├── test_quanta_glia_bdd.py      # (The existing shell script, can be phased out)
    └── test_knowledge_harvesting.py # A new BDD-style pytest file
```

### 3.2. Writing a BDD-Style Test File

Instead of separate `.feature` and `_steps.py` files, we write a single Python file. We use function names, docstrings, comments, and fixtures to structure the test in a readable, BDD format.

**File:** `tests/bdd/test_knowledge_harvesting.py`
```python
import os
import shutil
import subprocess
from pathlib import Path
import pytest

# To execute this test:
# 1.  Navigate to the root directory of the `quanta_glia` repository
# 2.  Run `pytest tests/bdd/test_quanta_glia_bdd.py`



# --- Fixtures (The "Given" part) ---

@pytest.fixture
def dummy_repo():
    """A pytest fixture that creates a temporary Git repository for testing."""
    repo_path = Path('./dummy_repo_for_bdd')
    if repo_path.exists():
        shutil.rmtree(repo_path)
    repo_path.mkdir()

    # Initialize Git and create some files
    subprocess.run(['git', 'init', '-b', 'main'], cwd=repo_path, check=True, capture_output=True)
    (repo_path / 'README.md').write_text('This is a test README.')
    (repo_path / 'LICENSE').write_text('This is a test LICENSE.')
    (repo_path / 'src.py').write_text('print("hello")') # A file that should not be harvested
    subprocess.run(['git', 'add', '.'], cwd=repo_path, check=True, capture_output=True)
    subprocess.run(['git', 'commit', '-m', 'Initial commit'], cwd=repo_path, check=True, capture_output=True)

    yield repo_path  # Provide the repo path to the test

    # --- Teardown ---
    shutil.rmtree(repo_path)

@pytest.fixture
def knowledge_base():
    """A fixture to manage the knowledge_base directory for a clean test run."""
    kb_path = Path('./knowledge_base')
    if kb_path.exists():
        shutil.rmtree(kb_path)

    yield kb_path  # Provide the path to the test

    # Teardown after the test is complete
    if kb_path.exists():
        shutil.rmtree(kb_path)


# --- Test Scenarios (The "When" and "Then" parts) ---

def test_harvesting_key_files_from_repository(dummy_repo, knowledge_base):
    """
    Scenario: Extracting key files from a single repository.
    """
    # GIVEN a Git repository (from the `dummy_repo` fixture)
    # AND a clean knowledge base (from the `knowledge_base` fixture)

    # WHEN I run the QuantaGlia script on that repository
    script_path = 'scripts/quanta_glia.py'
    repo_path = dummy_repo
    subprocess.run(['python3', script_path, str(repo_path)], check=True)

    # THEN the "knowledge_base" directory should contain the extracted files
    repo_kb_path = knowledge_base / repo_path.name
    assert repo_kb_path.exists(), "Knowledge base directory for the repo was not created"
    assert (repo_kb_path / 'README.md').exists(), "README.md was not extracted"
    assert (repo_kb_path / 'LICENSE').exists(), "LICENSE file was not extracted"

    # AND other files should not be extracted
    assert not (repo_kb_path / 'src.py').exists(), "Non-target file was incorrectly extracted"
```

### 3.3. BDD-Style `pytest` Best Practices

To keep tests readable and maintainable, we recommend the following conventions:

-   **Use Fixtures for `Given`:** `pytest` fixtures are the ideal way to handle setup conditions. Name them descriptively (e.g., `clean_knowledge_base`, `repo_with_no_readme`).

-   **Use `parametrize` for Data-Driven Tests:** To test the same behavior with different data (the equivalent of Gherkin's `Scenario Outline`), use the `@pytest.mark.parametrize` decorator.

    ```python
    @pytest.mark.parametrize("filename", ["README.md", "USAGE.txt", "ethics.rst"])
    def test_extracting_specific_files(filename):
        # GIVEN a repository contains a file named <filename>
        # WHEN I run the QuantaGlia script
        # THEN the knowledge base should contain the file <filename>
        ...
    ```

-   **Write Declarative Test Names:** The test function name should describe the scenario clearly. `test_harvesting_key_files_from_repository` is much clearer than `test_run_1`.

-   **Use Comments for `When/Then`:** Use simple comments inside the test function to delineate the action (`When`) and assertion (`Then`) parts of the test, making the flow obvious.

### 3.4. Advanced `pytest` Features

`pytest` offers powerful features for managing complex test suites:

-   **Fixture Scopes:** Fixtures can be configured to run once per function, class, module, or the entire test session (`@pytest.fixture(scope="session")`). This is perfect for managing expensive setup operations, like database connections or compiling a binary, which replaces the need for `behave`'s environment hooks.

-   **Markers:** You can add markers (e.g., `@pytest.mark.smoke`, `@pytest.mark.slow`) to your test functions. This allows you to selectively run a subset of your tests, which is useful for quick smoke tests or for separating unit and integration tests.

    ```python
    @pytest.mark.smoke
    def test_core_functionality():
        ...

    @pytest.mark.wip
    def test_a_new_feature():
        ...
    ```
    You can run tests with specific markers using the `-m` flag: `pytest -m smoke`.

---

## 4. Test Execution

With `pytest` installed (e.g., `pip install pytest`), the BDD test suite can be run with a single command from the root of the `quanta_glia` repository:

```bash
pytest tests/bdd/
```

This command will automatically discover and execute all `.feature` files within the specified directory.

---
## 5. Test Reporting

`behave` can generate test reports in various formats. For CI/CD integration, the JUnit XML format is particularly useful.

To generate a JUnit report, use the `--junit` flag:
```bash
behave tests/bdd/ --junit --junit-directory=reports/
```
This will create XML files in the `reports/` directory, which can be parsed and displayed by most CI/CD systems (like Jenkins, GitLab, or GitHub Actions).

---
## 6. CI/CD Integration

BDD tests should be integrated into a Continuous Integration (CI) pipeline to provide rapid feedback on every code change. Here is a conceptual example of a GitHub Actions workflow that runs the `behave` test suite:

**File:** `.github/workflows/bdd-tests.yml`
```yaml
name: BDD Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - name: Check out code
        uses: actions/checkout@v3

      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.x'

      - name: Install dependencies
        run: |
          python -m pip install --upgrade pip
          pip install pytest
          # pip install -r requirements.txt

      - name: Run BDD tests
        run: pytest tests/bdd/

      - name: (Optional) Run BDD tests with JUnit reporting
        if: always() # Run even if previous steps fail to generate a report
        run: pytest tests/bdd/ --junitxml=reports/report.xml

      - name: (Optional) Upload test reports
        if: always()
        uses: actions/upload-artifact@v3
        with:
          name: bdd-test-reports
          path: reports/report.xml
```

---

## 7. BDD for Future Features

This BDD framework provides a robust foundation for testing the future, aspirational features of QuantaGlia, such as:

- **Repository Pruning:**
  ```gherkin
  Scenario: Pruning an obsolete repository
    Given a repository in the knowledge base has not been used recently
    When the pruning cycle runs
    Then that repository should be archived
  ```

- **Repository Spawning:**
  ```gherkin
  Scenario: Spawning a new utility script
    Given the system detects repeated command failures
    When the repository spawner is triggered
    Then a new repository with a utility script should be created
  ```

By defining these behaviors upfront in Gherkin, we can ensure that development stays aligned with the project's goals and that new features are testable from day one.
