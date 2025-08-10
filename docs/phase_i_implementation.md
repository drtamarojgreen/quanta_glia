#  Phase 1 Implementation Plan: Foundational Pruner

This document outlines the implementation plan for Phase 1 of the QuantaGlia-Pruner module. It converges the goals from the original `plan.md` with the specific, actionable items from `recommendations.md` and `enhancements.md` to provide a clear path for development.

---

## 1. Core Objective

The primary goal of Phase 1 is to create a **Foundational Pruner** that operates on simple, configurable heuristics. This initial version prioritizes safety, logging, and predictability, establishing a solid base for future enhancements.

---

## 2. Key Features

Phase 1 will deliver the following key features:

-   **Core Pruner Script (`pruner.py`):** A new script responsible for all pruning logic.
-   **Knowledge Base Scanner:** The script will read and parse the `knowledge_base/` directory to identify repositories for evaluation.
-   **Configuration-Driven Logic:** All pruning parameters will be loaded from `config.yaml`, ensuring that behavior is easily configurable without code changes.
-   **Age-Based Pruning Rule:** The primary heuristic for this phase will be the age of a repository. The pruner will identify and flag repositories older than a configurable `age_threshold_days`.
-   **Safe-by-Default Archiving:** To prevent accidental data loss, the default action for a flagged repository will be to "archive" it (e.g., rename or move), not to delete it permanently.
-   **Dry-Run Mode:** A critical safety feature. When run with the `--dry-run` flag, the pruner will report the actions it *would* take without making any actual changes to the filesystem.
-   **Structured Logging:** All decisions and actions will be logged in a structured format to `quantaglia.log`, providing a clear audit trail.

---

## 3. Command-Line Interface (CLI)

The `pruner.py` script will support the following command-line arguments:

```bash
# Perform a dry run, showing what would be pruned without making changes
python scripts/pruner.py --dry-run

# Force a run, ignoring the schedule in the config (if scheduling is active)
python scripts/pruner.py --force

# Increase logging verbosity for debugging
python scripts/pruner.py --verbose
```

---

## 4. Configuration (`config.yaml`)

The pruner's behavior will be controlled by a dedicated section in the `config.yaml` file.

**Example Schema:**
```yaml
pruning:
  # The interval in minutes for periodic pruning runs.
  interval_minutes: 1440 # 24 hours

  # Repositories older than this will be considered for pruning.
  age_threshold_days: 90

  # The default strategy for the pruner.
  # "conservative": Archives repositories. Deletion is disabled.
  # "aggressive": Allows deletion of repositories.
  default_strategy: "conservative"

  # Path to the directory where archived repositories are moved.
  archive_path: "repo_archive/"
```

---

## 5. Logging and Auditing

To ensure transparency and traceability, all operations will be logged in a structured, machine-readable format.

**Example Log Entry (JSONL format):**
```json
{"timestamp": "2023-10-27T10:00:00Z", "event": "prune_decision", "repo_name": "old-project", "decision": "ARCHIVE", "reason": "Exceeded age threshold of 90 days", "age_days": 120, "dry_run": false, "actor": "QuantaGlia-Pruner"}
```

---

## 6. Testing Strategy

As per the `unit_testing_plan.md`, new components introduced in this phase will be accompanied by unit tests following the standard conventions:

-   **Framework:** Python's built-in `unittest` framework.
-   **Location:** `tests/unit/`.
-   **Focus:** Test individual functions in isolation, such as `calculate_repo_age`, `load_config`, and the decision logic for pruning.
-   **Mocking:** Dependencies on the filesystem will be mocked to ensure tests are hermetic and do not have side effects, addressing the concerns raised in `testing_challenges.md`.

Integration tests will verify the end-to-end functionality, including the correct behavior of the `--dry-run` flag. These tests will use temporary, locally created Git repositories as fixtures to avoid external network dependencies.
