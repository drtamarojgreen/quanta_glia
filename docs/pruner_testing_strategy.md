# Testing Strategy for the Foundational Pruner (`pruner.py`)

This document outlines the comprehensive testing strategy for the `pruner.py` script, as defined in the `phase_i_implementation.md`. The goal is to ensure the pruner is reliable, predictable, and robust by validating its logic in isolation (unit tests) and its behavior in a controlled, end-to-end environment (integration tests).

This strategy adheres to the conventions established in `docs/unit_testing_plan.md` and directly addresses the issues raised in `docs/testing_challenges.md` by eliminating external dependencies.

---

## 1. Guiding Principles

-   **Hermetic Tests:** All tests must be self-contained and must not depend on external services, network access, or a pre-existing file structure.
-   **Mocking over Real Dependencies:** Filesystem interactions, configuration loading, and logging will be mocked in unit tests to ensure isolation.
-   **Fixtures for Integration Tests:** Integration tests will programmatically create and destroy their own temporary Git repositories and configuration files as fixtures for each test run.
-   **Clarity and Maintainability:** Tests will follow the Arrange-Act-Assert pattern and use descriptive naming conventions as specified in the project's unit testing plan.

---

## 2. Unit Testing (`tests/unit/test_pruner.py`)

A new test file, `tests/unit/test_pruner.py`, will be created to house all unit tests for the `pruner.py` script.

### 2.1. Scope and Objectives

The primary objective of the unit tests is to verify the correctness of individual functions and logic blocks within the pruner script in complete isolation.

### 2.2. Key Areas to Test

-   **Configuration Loading:**
    -   Test that the `load_config` function correctly parses a valid `config.yaml`.
    -   Test its behavior with a missing or malformed `config.yaml`.
    -   Use `unittest.mock.patch` to mock `open` and `yaml.safe_load` to avoid real file I/O.

-   **Repository Age Calculation:**
    -   Test the `calculate_repo_age` function.
    -   Mock the underlying filesystem call (e.g., `os.path.getmtime`) to simulate repositories of various ages (e.g., newer than, older than, and exactly on the age threshold).

-   **Pruning Decision Logic:**
    -   Test the core decision-making function that determines whether a repository should be archived.
    -   Provide it with various mock repository objects and configurations (`conservative` vs. `aggressive` strategies) and assert that it returns the correct decision (`ARCHIVE`, `DELETE`, or `KEEP`).

-   **Filesystem Actions:**
    -   Test the functions responsible for performing actions (e.g., `archive_repository`).
    -   Use `unittest.mock.patch` to mock the filesystem calls (e.g., `os.rename` or `shutil.move`) and assert that they are called with the correct arguments based on the pruning decision.

---

## 3. Integration Testing (`tests/integration/test_pruner_integration.py`)

A new integration test file, `tests/integration/test_pruner_integration.py`, will be created to verify the end-to-end functionality of the `pruner.py` script when executed as a command-line tool.

### 3.1. Test Fixtures

A `unittest.TestCase` class will manage the test environment using `setUp()` and `tearDown()` methods.
-   **`setUp()`**:
    1.  Create a temporary root directory for the test run using the `tempfile` module.
    2.  Inside it, create a `knowledge_base/` directory.
    3.  Programmatically initialize several local Git repositories inside `knowledge_base/` using `subprocess`. Modify their metadata to simulate different ages.
    4.  Create a mock `config.yaml` file with specific settings for the test case (e.g., `age_threshold_days`, `archive_path`).
-   **`tearDown()`**:
    1.  Recursively delete the temporary root directory and all its contents.

### 3.2. Key Scenarios to Test

-   **Dry Run Mode (`--dry-run`):**
    -   Run `pruner.py --dry-run`.
    -   Assert that the log output correctly identifies which repositories *would* be archived.
    -   Assert that no actual file operations occurred (i.e., no repositories were moved).

-   **Conservative Pruning (Live Run):**
    -   Run `pruner.py` with a "conservative" strategy in the config.
    -   Assert that only the repositories older than the threshold are moved to the specified `archive_path`.
    -   Assert that younger repositories remain untouched.

-   **Logging Verification:**
    -   For all test cases, capture the logger's output (e.g., by redirecting it to a `StringIO` buffer).
    -   Assert that the logs are structured correctly (JSONL format) and contain the expected information for each decision.

-   **Edge Cases:**
    -   Test behavior when no repositories meet the pruning criteria.
    -   Test behavior when the `knowledge_base` directory is empty or does not exist.

---

## 4. Test Execution

As per `scripts/test_all.sh`, the new unit tests in `tests/unit/` will be automatically discovered and run. A similar command should be added to the appropriate test script to execute the new integration tests.

Example command to add to `scripts/test_all.sh` or a similar script:
```bash
echo "🔬 Running integration tests for pruner..."
python3 -m unittest discover tests/integration/
```
