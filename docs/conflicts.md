# Documentation Conflicts and Inconsistencies

This document lists the identified conflicts and inconsistencies between the project's documentation and the actual state of the codebase.

---

### 1. Pruner Implementation Status

-   **Conflict**: The `README.md` file states that the "Pruner" feature is "not yet implemented." However, `docs/plan.md` provides a detailed implementation plan for the pruner, and a `scripts/pruner.py` script exists.
-   **Files**:
    -   `README.md`
    -   `docs/plan.md`
    -   `scripts/pruner.py`

---

### 2. Bootstrap Script Functionality

-   **Conflict**: The `docs/workplace_testing_plan.md` file claims that `scripts/bootstrap.sh` is "currently empty" and that the automated setup described in the `README.md` is "not yet functional." However, `scripts/bootstrap.sh` is a fully functional script.
-   **Files**:
    -   `docs/workplace_testing_plan.md`
    -   `scripts/bootstrap.sh`
    -   `README.md`

---

### 3. Script Execution Method

-   **Conflict**: The `docs/workplace_testing_plan.md` file instructs users to manually edit the `example_repos` list in `scripts/quanta_glia.py` to run tests. However, the script is designed to be run with command-line arguments, as stated in the `README.md` and implemented in the script's `if __name__ == "__main__":` block.
-   **Files**:
    -   `docs/workplace_testing_plan.md`
    -   `scripts/quanta_glia.py`
    -   `README.md`

---

### 4. BDD Test File Type

-   **Conflict**: The `docs/bdd_testing_plan.md` file incorrectly identifies `tests/bdd/test_quanta_glia_bdd.py` as a shell script. The file is a Python script.
-   **Files**:
    -   `docs/bdd_testing_plan.md`
    -   `tests/bdd/test_quanta_glia_bdd.py`

---

### 5. Unit Test Execution

-   **Conflict**: The `docs/unit_testing_plan.md` file recommends adding the command to run unit tests (`python3 -m unittest discover tests/unit/`) to the `scripts/test_all.sh` script. This command is missing from `scripts/test_all.sh`.
-   **Files**:
    -   `docs/unit_testing_plan.md`
    -   `scripts/test_all.sh`

---

### 6. Test Suite Automation Status

-   **Conflict**: The `README.md` describes a fully automated test suite (`scripts/test_all.sh`). However, `docs/workplace_testing_plan.md` provides manual testing instructions, stating that the automated scripts are "currently empty." This contradicts the state of `scripts/test_all.sh` and `scripts/bootstrap.sh`.
-   **Files**:
    -   `README.md`
    -   `docs/workplace_testing_plan.md`
    -   `scripts/test_all.sh`
    -   `scripts/bootstrap.sh`
