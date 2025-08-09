# BDD Testing Plan for QuantaGlia

This document outlines the strategy for Behavior-Driven Development (BDD) testing within the QuantaGlia project. It covers the current approach, proposes a more robust future-state framework, and provides actionable examples for implementation.

---

## 1. Introduction to BDD

Behavior-Driven Development (BDD) is a software development methodology that encourages collaboration between developers, quality assurance (QA) professionals, and business stakeholders. It aims to create a shared understanding of how an application should behave by using a natural, human-readable language.

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

## 3. Proposed BDD Framework

To enhance our BDD capabilities, this plan proposes the adoption of a BDD framework for Python.

**Why a BDD Framework?**
- **Native Python:** Integrates seamlessly with the existing codebase.
- **Structured:** Enforces a clear separation between feature specifications and test code.
- **Readable:** Feature files act as living documentation.
- **Extensible:** Supports fixtures, tagging, and detailed reporting.

### 3.1. Proposed Directory Structure

We will adopt a standard directory structure within the existing `tests/bdd/` directory:

```text
tests/
└── bdd/
    ├── features/
    │   ├── environment.py
    │   ├── harvest_knowledge.feature
    │   └── steps/
    │       └── harvesting_steps.py
    └── test_quanta_glia_bdd.py  # (The existing shell script)
```

- **`features/`**: This directory will contain all feature files.
- **`features/steps/`**: This directory will contain the Python step definition files that implement the steps in the feature files.
- **`features/environment.py`**: This optional file is used for test hooks.

### 3.2. Writing a Feature File

Here is an example feature file that describes the current core functionality of QuantaGlia.

**File:** `tests/bdd/features/harvest_knowledge.feature`
```
Feature: Knowledge Harvesting
  As a user, I want to run QuantaGlia on a repository
  so that key informational files are extracted into the knowledge base.

  Scenario: Extracting key files from a single repository
    Given a Git repository exists at a local path
    When I run the QuantaGlia script on that repository
    Then the "knowledge_base" directory should contain the extracted files
```

### 3.3. Writing Step Definitions

The steps in the feature file are mapped to Python functions in a step definition file.

**File:** `tests/bdd/features/steps/harvesting_steps.py`
```python
import os
import shutil
import subprocess
from pathlib import Path

# --- Setup and Teardown ---
def setup_a_git_repository(context):
    context.repo_path = Path('./dummy_repo_for_bdd')
    if context.repo_path.exists():
        shutil.rmtree(context.repo_path)
    context.repo_path.mkdir()
    
    subprocess.run(['git', 'init', '-b', 'main'], cwd=context.repo_path, check=True)
    (context.repo_path / 'README.md').write_text('This is a test README.')
    (context.repo_path / 'LICENSE').write_text('This is a test LICENSE.')
    subprocess.run(['git', 'add', '.'], cwd=context.repo_path, check=True)
    subprocess.run(['git', 'commit', '-m', 'Initial commit'], cwd=context.repo_path, check=True)

def run_the_quantaglia_script(context):
    script_path = 'scripts/quanta_glia.py'
    context.kb_path = Path('./knowledge_base')
    
    # Clean up previous runs
    if (context.kb_path / context.repo_path.name).exists():
        shutil.rmtree(context.kb_path / context.repo_path.name)
        
    subprocess.run(['python3', script_path, str(context.repo_path)], check=True)

def verify_extracted_files(context):
    repo_kb_path = context.kb_path / context.repo_path.name
    assert repo_kb_path.exists()
    assert (repo_kb_path / 'README.md').exists()
    assert (repo_kb_path / 'LICENSE').exists()
    
    # --- Teardown ---
    shutil.rmtree(context.repo_path)
    shutil.rmtree(repo_kb_path)
```

### 3.4. BDD Best Practices

To keep feature files readable and maintainable, we recommend following these best practices:

-   **Use `Background` for common setup:** If multiple scenarios in the same feature file share the exact same `Given` steps, you can move them into a `Background` section. This reduces duplication and makes the scenarios cleaner.
    ```
    Background:
      Given a clean knowledge base
      And a local git repository named "my-project" exists
    ```

-   **Use `Scenario Outline` for data-driven tests:** When you need to test the same behavior with different sets of data, use a `Scenario Outline` with an `Examples` table.
    ```
    Scenario Outline: Extracting specific files
      Given the repository contains a file named "<filename>"
      When I run the QuantaGlia script on the repository
      Then the knowledge base should contain the file "<filename>"

      Examples:
        | filename      |
        | README.md     |
        | USAGE.txt     |
        | contributing.rst |
    ```

-   **Write in a declarative style:** Focus on *what* the system should do, not *how* it does it. Avoid describing UI interactions or implementation details in your steps.
    -   **Imperative (Bad):** `When I type "my-repo" into the "repo_url" field and click the "harvest" button`
    -   **Declarative (Good):** `When I run the QuantaGlia script on that repository`

### 3.5. Advanced BDD Features

A BDD framework can offer powerful features for managing complex test suites:

-   **Hooks:** You can define "hooks" that run before or after certain events (e.g., before a scenario runs, after all tests are done). These are placed in a special `environment.py` file. This is useful for setting up database connections, cleaning up test data, or managing browser sessions.
    ```python
    # environment.py
    def before_scenario(context, scenario):
        print(f"Starting scenario: {scenario.name}")
        # Code to set up a clean state before each scenario
    
    def after_scenario(context, scenario):
        print(f"Finished scenario: {scenario.name}")
        # Code to tear down or clean up resources
    ```

-   **Tags:** You can add tags (e.g., `@smoke`, `@wip`) to your features or scenarios. This allows you to selectively run a subset of your tests, which is useful for quick smoke tests or for focusing on work-in-progress features.
    ```
    @smoke @fast
    Feature: Core Functionality

    @wip
    Scenario: A new feature being developed
    ```
    You can run tests with specific tags using a command-line flag.

---

## 4. Test Execution

The BDD test suite can be run with a single command from the root of the `quanta_glia` repository. This command will automatically discover and execute all feature files within the specified directory.

---
## 5. Test Reporting

A BDD framework can generate test reports in various formats. For CI/CD integration, a standard XML format is particularly useful.

This will create XML files in a `reports/` directory, which can be parsed and displayed by most CI/CD systems.

---
## 6. CI/CD Integration

BDD tests should be integrated into a Continuous Integration (CI) pipeline to provide rapid feedback on every code change.

---

## 7. BDD for Future Features

This BDD framework provides a robust foundation for testing the future, aspirational features of QuantaGlia. By defining these behaviors upfront, we can ensure that development stays aligned with the project's goals and that new features are testable from day one.
