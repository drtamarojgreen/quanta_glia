# 📘 Module Planning Document

## Module Name
- QuantaGlia-Pruner

## Purpose
- To implement the intelligent, automated maintenance capabilities of the QuantaGlia system, as described in the `README.md`.
- This module will be responsible for periodically evaluating repositories for redundancy, obsolescence, and low impact, and then merging, archiving, or deleting them based on configurable thresholds. This is a core component of the system's automated maintenance function.

## Goals
- [x] **Define inputs and outputs**
  - **Inputs:**
    - `knowledge_base/`: The Pruner will scan this directory to evaluate the extracted knowledge from each repository.
    - `quantaglia.log`: The log file will be used to determine repository age and usage patterns (e.g., how often a repository's knowledge is accessed, although this is a future enhancement).
    - `config.yaml`: A configuration file specifying the pruning thresholds (e.g., `age_threshold_days`, `min_usage_score`).
  - **Outputs:**
    - Pruning actions (merge, archive, delete) performed on the repositories in the `knowledge_base` and `repo_cache`.
    - Detailed logs in `quantaglia.log` with justifications for each pruning decision.
- [x] **Define triggers and dependencies**
  - **Triggers:** The pruner will be triggered periodically, based on a schedule defined in the `config.yaml` (e.g., `pruning.interval_minutes`). This will be managed by the `QuantaParent` scheduler.
  - **Dependencies:**
    - Python 3.x
    - PyYAML for reading the configuration file.
- [ ] **Establish ethical decision logic (if applicable)**
  - Pruning decisions, especially deletion, are destructive. Therefore, the Pruner must integrate with `QuantaEthos`.
  - Deletion actions must be logged with a high level of detail.
  - An "archive" instead of "delete" policy will be the default for repositories that are not clearly redundant or useless, to prevent accidental knowledge loss.

## Integration Plan
- How will this module communicate with:
  - 🧠 **QuantaSensa:** QuantaSensa may provide signals about which knowledge is being used, which can inform the Pruner's `min_usage_score`.
  - 🕊️ **QuantaEthos:** The Pruner must consult QuantaEthos before any destructive operation (delete/merge). Ethical guardrails will prevent the deletion of critical or ethically sensitive knowledge.
  - 🧬 **QuantaSynapse:** The Pruner will update the QuantaSynapse knowledge graph to reflect the changes in the knowledge base (e.g., removing nodes for pruned repositories).
  - 🧹 **QuantaGlia:** The Pruner is a core component of QuantaGlia. It will work alongside the existing repository cloner and information extractor.

## Notes
- **Anticipated constraints:**
  - The logic for determining "redundancy" and "impact" can be complex. The initial version will rely on simple heuristics (e.g., age, presence of key files).
- **Potential edge cases:**
  - Handling of repositories that are part of a dependency chain. Pruning a repository that another depends on could cause issues. A dependency tracking mechanism will be needed.
