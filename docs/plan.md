# 📘 Module Planning Document: QuantaGlia-Pruner

## Module Name
- QuantaGlia-Pruner

## Purpose
- To implement the intelligent, automated maintenance capabilities of the QuantaGlia system, as described in the `README.md`.
- This module will be responsible for periodically evaluating repositories for redundancy, obsolescence, and low impact, and then merging, archiving, or deleting them based on configurable thresholds. This is a core component of the system's automated maintenance function.

## Goals
- [x] **Define inputs and outputs**
  - **Inputs:**
    - `knowledge_base/`: The Pruner will scan this directory to evaluate the extracted knowledge from each repository.
    - `quantaglia.log`: The log file will be used to determine repository age and potentially usage patterns (future enhancement).
    - `config.yaml`: A configuration file specifying the pruning thresholds (e.g., `age_threshold_days`, `min_usage_score`).
    - **(Future)** Signals from `QuantaSensa` regarding knowledge usage frequency.
    - **(Future)** Dependency manifests (`requirements.txt`, `package.json`, etc.) within repositories.
  - **Outputs:**
    - Pruning actions (merge, archive, delete) performed on the repositories in the `knowledge_base` and `repo_cache`.
    - Detailed logs in `quantaglia.log` with justifications for each pruning decision.
    - **(Future)** Webhook notifications to external systems.
    - **(Future)** Updates to the `QuantaSynapse` knowledge graph.
- [x] **Define triggers and dependencies**
  - **Triggers:** The pruner will be triggered periodically, based on a schedule defined in the `config.yaml` (e.g., `pruning.interval_minutes`). This will be managed by the `QuantaParent` scheduler.
  - **Dependencies:**
    - Python 3.x
- [ ] **Establish ethical decision logic (if applicable)**
  - Pruning decisions, especially deletion, are destructive. Therefore, a robust ethical framework is not optional; it is required.
  - All deletion actions must be logged with a high level of detail, including the justification and the ethical clearance received.
  - An "archive" instead of "delete" policy will be the default for repositories that are not clearly redundant or useless, to prevent accidental knowledge loss.
  - Implement a "dry run" mode to simulate actions without making changes.
  - Implement a "human-in-the-loop" mechanism to flag borderline cases for manual review.
  - **Ethical Framework Integration:**
    - The pruner's decision-making will be governed by the principles in `quanta_glia_checkpoints.md`.
    - Specifically, it must pass the **Knowledge-Driven Pruning Checkpoint (9)**.
    - Any destructive action (delete) must be scored against the **Priority Policy Matrix**, with a high threshold required for `Ethical Compliance` and `Privacy & Compliance`.
    - The final decision score will be calculated using the **Weighted Model Decision Algorithm**.

## Integration Plan
- How will this module communicate with:
  - 🧠 **QuantaSensa:** QuantaSensa may provide signals about which knowledge is being used, which can inform the Pruner's `min_usage_score`. The Pruner will query Sensa for a "usage score" for a given knowledge artifact.
  - 🕊️ **QuantaEthos:** The Pruner must consult QuantaEthos before any destructive operation (delete/merge). It will submit a "pruning proposal" containing the target repository and the justification. Ethos will return an approval or rejection based on its policy matrix. Ethical guardrails will prevent the deletion of critical or ethically sensitive knowledge.
  - 🧬 **QuantaSynapse:** The Pruner will update the QuantaSynapse knowledge graph to reflect the changes in the knowledge base (e.g., removing nodes for pruned repositories). This ensures the system's map of its own knowledge remains accurate.
  - 🧹 **QuantaGlia:** The Pruner is a core component of QuantaGlia. It will work alongside the existing repository cloner and information extractor.

## Notes
- **Anticipated constraints:**
  - The logic for determining "redundancy" and "impact" can be complex. The initial version will rely on simple heuristics (e.g., age, presence of key files).
- **Potential edge cases:**
  - Handling of repositories that are part of a dependency chain. Pruning a repository that another depends on could cause issues. A dependency tracking mechanism will be needed.
  - A repository might be old but contain foundational, unchanging knowledge (e.g., a core algorithm). Age alone is an insufficient metric.

---

## Command-Line Interface (CLI)

The `pruner.py` script will support the following command-line arguments to provide flexibility and control.

```bash
# Perform a dry run, showing what would be pruned without making changes
python scripts/pruner.py --dry-run

# Run in aggressive mode, allowing deletion instead of just archiving
python scripts/pruner.py --strategy aggressive

# Force a run, ignoring the schedule in the config
python scripts/pruner.py --force

# Increase logging verbosity
python scripts/pruner.py --verbose
```

## Testing Strategy

- **Unit Tests:** Each function (e.g., `calculate_repo_score`, `load_config`) will have dedicated unit tests. Test files will be located in a `tests/` directory.
- **Integration Tests:** A suite of tests will simulate the `knowledge_base` with various scenarios (old repos, empty repos, protected repos). These tests will verify that the pruner correctly identifies targets and that `--dry-run` mode works as expected.
- **Workspace Test:** The `test_workspace.sh` script will be updated to include a call to the pruner's test suite, ensuring it is run as part of the full system check.

---

## Implementation Phases

### Phase 1: Foundational Pruner (Heuristic-Based)
- [x] Create the core `pruner.py` script.
- [x] Implement logic to read the `knowledge_base` directory.
- [x] Implement a simple age-based pruning rule (e.g., archive repos older than `age_threshold_days` from `config.yaml`).
- [x] Add robust logging for all decisions and actions.
- [x] Implement a "dry-run" command-line flag (`--dry-run`) to report what would be pruned.

### Phase 2: Enhanced Logic & Safety
- [x] Develop a more sophisticated scoring model that combines multiple heuristics:
  - Age of repository.
  - Presence/absence of key files (README, LICENSE, etc.).
  - Size and complexity (lines of code, number of files).
- [x] Implement configurable pruning strategies in `config.yaml` (e.g., `conservative`, `aggressive`).
- [x] Default to "archive" instead of "delete" for all operations.

### Phase 3: Ethical Integration
- [ ] Develop the API contract for communicating with `QuantaEthos`.
- [ ] Implement the logic to send a "pruning proposal" to `QuantaEthos` before taking action.
- [ ] Implement the "human-in-the-loop" feature: if `QuantaEthos` returns an "uncertain" response, flag the repo for manual review and take no action.

### Phase 4: Advanced Intelligence
- [ ] Integrate with `QuantaSensa` to factor in real usage data into the pruning score.
- [ ] Begin development of a dependency analysis module to parse dependency files to build a dependency graph and prevent pruning of critical libraries.
- [ ] Explore NLP techniques for similarity analysis to identify and flag redundant repositories for merging.
