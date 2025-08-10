# Phase II Implementation Plan: Intelligent Pruner & Ethical Integration

This document outlines the implementation plan for Phase II of the QuantaGlia-Pruner module. It builds upon the foundational pruner created in Phase I, introducing more sophisticated decision-making, configurable strategies, and the initial integration with the ethical framework.

---

## 1. Core Objective

The primary goal of Phase II is to elevate the pruner's intelligence by replacing the simple age-based heuristic with a **multi-factor composite scoring model**. This phase also introduces configurable pruning strategies and lays the architectural groundwork for integration with `QuantaEthos`.

---

## 2. Key Features

Phase II will deliver the following key features:

-   **Composite Scoring Model:** The pruner will evaluate repositories using a weighted score calculated from multiple factors, such as age, usage (mocked for now), redundancy (mocked), and ethical risk (mocked).

-   **Configurable Strategies:** The pruner will support different operational strategies (`conservative` vs. `aggressive`) that can be set in `config.yaml` and overridden via a command-line argument. The `aggressive` strategy will enable permanent deletion of repositories.

-   **Typed Data Models:** The project will adopt Python's `dataclasses` to create strongly-typed models for `RepoRecord` and `PruneDecision`, improving code clarity and reliability as recommended.

-   **Ethical Integration Hooks:** The pruner's logic will be updated to formulate a "pruning proposal" and prepare for communication with `QuantaEthos`. The actual communication will be mocked in this phase.

-   **Human-in-the-Loop Flagging:** A mechanism will be added to flag repositories for manual review if their prune score falls within a "review" threshold, preventing automated action on borderline cases.

---

## 3. Command-Line Interface (CLI)

The `pruner.py` script will be enhanced with a new argument to control the pruning strategy for a specific run.

```bash
# Run the pruner using an aggressive strategy, allowing deletion
python scripts/pruner.py --strategy aggressive
```

---

## 4. Configuration (`config.yaml`)

The `pruning` section of `config.yaml` will be expanded to support the new composite scoring model with configurable weights and decision thresholds.

**Example Schema:**
```yaml
pruning:
  # ... existing settings ...
  default_strategy: "conservative"
  archive_path: "repo_archive/"

  # Weights for the composite score calculation
  scoring_weights:
    usage: 0.5
    age: 0.2
    redundancy: 0.2
    ethics_risk: 0.1

  # Decision thresholds for the final score
  decision_thresholds:
    delete: 0.9  # Score >= 0.9 -> DELETE (if strategy is aggressive)
    archive: 0.7 # Score >= 0.7 -> ARCHIVE
    review: 0.5  # Score >= 0.5 -> Flag for human review
                 # Score < 0.5 -> KEEP
```

---

## 5. Core Data Models

To improve architectural clarity, we will introduce typed data models.

```python
from dataclasses import dataclass
from datetime import datetime
from typing import List, Literal

DecisionType = Literal["DELETE", "ARCHIVE", "REVIEW", "KEEP"]

@dataclass
class RepoRecord:
    name: str
    path: str
    last_activity_at: datetime
    # Mocked scores for Phase II
    usage_score: float = 0.0
    redundancy_score: float = 0.0
    ethics_risk_score: float = 0.0

@dataclass
class PruneDecision:
    repo_name: str
    decision: DecisionType
    final_score: float
    reasons: List[str]
    dry_run: bool
    policy_verdict: Literal["ALLOW", "DENY"]
```

---

## 6. Testing Strategy

The testing strategy will be expanded to cover the new features:

-   **Unit Tests (`tests/unit/test_pruner.py`):**
    -   Test the `calculate_composite_score` function with various mock `RepoRecord` objects.
    -   Test the `make_pruning_decision` function to ensure it correctly applies the thresholds from the config.
    -   Test the handling of the `conservative` and `aggressive` strategies.

-   **Integration Tests (`tests/integration/test_pruner_integration.py`):**
    -   Add a new test case to verify the `--strategy aggressive` flag correctly enables deletion.
    -   Add a test case to verify that repositories are flagged for "review" when their score falls within the review threshold.
    -   Verify that the structured logs correctly report the composite score and the final decision.