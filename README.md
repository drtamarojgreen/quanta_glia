# QuantaGlia: Dynamic Knowledge Pruner

> **Disclaimer:** This README describes the aspirational goals and architecture of the QuantaGlia project. The current implementation is in an early stage. Key foundational pieces like the multi-repository **bootstrapper** (`scripts/bootstrap.sh`) and an initial version of the **pruner** (`scripts/pruner.py`) are functional. However, many advanced features described below (such as repository spawning and semantic annotation) are not yet implemented.

---

## 🚀 Quickstart

Get up and running with QuantaGlia in a few simple steps.

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/drtamarojgreen/quanta_glia.git
    cd quanta_glia
    ```

2.  **Run the bootstrap script:**
    This script sets up the necessary environment and dependencies.
    ```bash
    bash scripts/bootstrap.sh
    ```

3.  **Perform a dry run of the pruner:**
    This command will simulate the pruning process without making any actual changes.
    ```bash
    python3 scripts/pruner.py --dry-run
    ```

---

QuantaGlia is a modular subsystem within the PrismQuanta framework designed to autonomously collect, curate, and evolve knowledge repositories through intelligent spawning and pruning. It ensures the knowledge base remains relevant, efficient, and focused on the mission at hand.

---

## 🚀 Workspace Setup and Testing

This project serves as the primary entry point for setting up and testing the entire PrismQuanta ecosystem.

### Workflow and Directory Structure

The bootstrapping and testing process is designed to create a clean, multi-repository workspace.

1.  **Initial Clone**: First, clone the `quanta_glia` repository into a dedicated workspace directory.
    ```bash
    git clone https://github.com/drtamarojgreen/quanta_glia.git
    ```

2.  **Run the Bootstrapper**: Navigate into the new directory and execute the `bootstrap.sh` script.
    ```bash
    cd quanta_glia
    bash scripts/bootstrap.sh
    ```

3.  **Run Tests**:
    After a future implementation of the `bootstrap.sh` script, the workspace would have the following structure:
    ```text
    workspace/
    ├── prismquanta/     (Core framework)
    ├── quanta_alarma/   (Alerting/Monitoring)
    ├── quanta_cerebra/  (Coordination/Orchestration)
    ├── quanta_cogno/    (Cognitive modeling)
    ├── quanta_dorsa/    (Data persistence/backbone)
    ├── quanta_ethos/    (Ethical governance)
    ├── quanta_glia/     (Knowledge curation) <-- You are here
    ├── quanta_lista/    (Task management)
    ├── quanta_memora/   (Memory management)
    ├── quanta_porto/    (I/O and communication)
    ├── quanta_pulsa/    (Heartbeat/System status)
    ├── quanta_retina/   (Observation/Vision)
    ├── quanta_sensa/    (Sensory input/Workflow execution)
    ├── quanta_serene/   (State management/Serenity)
    ├── quanta_synapse/  (Inter-agent communication)
    └── quanta_tissu/    (Resource management/Fabric)
    ```

    At that point, the entire test suite could be run from the `quanta_glia` directory using a convenience script:
    ```bash
    ./test_workspace.sh
    ```

## Core Purpose

QuantaGlia simulates the behavior of biological glial cells: maintaining system health by pruning unused connections, fostering the growth of productive knowledge, and dynamically reorganizing structures based on learning outcomes.

---

## Key Functions

### 1. **Repository Spawner**
> **Status:** Not yet implemented. This section describes a planned feature.

- Clones or forks new lightweight repositories based on identified gaps in knowledge.
- Automatically tags each repo with intent, utility, and dependencies.
- Supports different repo types (code, documentation, datasets).

### 2. **Pruner**
> **Status:** An initial version of the pruner is functional. See `scripts/pruner.py`.

- Periodically evaluates repositories for redundancy, obsolescence, or low impact.
- Merges, archives, or deletes based on configurable thresholds.
- Logs all pruning decisions with justification.

### 3. **Semantic Annotator**
> **Status:** Not yet implemented. This section describes a planned feature.

- Annotates code, scripts, and documentation with contextual tags.
- Improves discoverability and thematic coherence of the knowledge base.

### 4. **Knowledge Curator**
> **Status:** Not yet implemented. This section describes a planned feature.

- Aggregates similar repositories into unified knowledge bundles.
- Identifies contradictory or outdated knowledge and flags it for review.

### 5. **Trigger-Based Behavior**
> **Status:** Not yet implemented. This section describes a planned feature.

- Learns from agent activity (e.g., frequent command patterns, repeated questions).
- Spawns new utilities (scripts, man-page digests, analysis tools) in response.

---

## Enhancement Roadmap

### 🧠 Learning Capabilities

- Integrate reinforcement signals from QuantaSensa and QuantaEthos.
- Refine spawning/pruning thresholds using weighted ethical factors.

### 🔄 Recursive Knowledge Bootstrapping

- Enable QuantaGlia to recursively improve its own strategies by analyzing the outcomes of prior spawn/prune cycles.

### 🕸️ Distributed Pruning Clusters

- Support federated deployments where multiple QuantaGlia nodes coordinate.
- Ideal for multi-agent or distributed environments.

### ⚖️ Ethical Regulation Tuning

- Work with QuantaEthos to ensure that spawned repos align with ethical guardrails.

### 🔍 Visual Interface

- Develop a web-based dashboard to visualize pruning cycles, knowledge flows, and current repo health.

---

## System Integration

| Component    | Interaction Type    | Description                                     |
| ------------ | ------------------- | ----------------------------------------------- |
| QuantaSensa  | Learns from actions | Detects needs based on workflow patterns        |
| QuantaEthos  | Ethics enforcement  | Rejects or flags unethical knowledge branches   |
| QuantaParent | Schedules execution | Manages spawn/prune intervals and system load   |
| LLaMA.cpp    | LLM backend         | Uses internal LLM to reason about knowledge use |

---

## Sample Configuration (YAML)

> **Note:** The configuration is not yet integrated into the main script. The `config.yaml` file contains the following placeholder values.

```yaml
# Configuration for QuantaGlia Pruner
pruning:
  age_threshold_days: 30
```

### Future Configuration Example

```yaml
pruning:
  interval_minutes: 90
  age_threshold_days: 30
  min_usage_score: 3
  auto_merge_similars: true
  ethical_filter: true

spawning:
  trigger_types: ["repeated_failures", "knowledge_gap", "missing_util"]
  repo_types: ["script", "doc", "tool"]
  sandbox_only: true
```

---

## Naming Convention

- `glia-<function>-<timestamp>` for spawned utilities
- `archive-<repo>-<pruned-reason>` for pruned bundles

---

## License & Compliance

- All activity is subject to PrismQuanta's ethical and audit policies.
- Repositories are to remain in the sandbox unless human-reviewed.

---

## Final Thought

QuantaGlia is more than a tool—it's a neural helper, sustaining a logical and evolving digital ecosystem. When tuned with ethical priorities and thoughtful constraints, it ensures that only the most valuable knowledge survives and grows.

---

**Maintained by:** PrismQuanta Lab\
**Version:** 0.9.1 (Pre-Federated)

---

> "Glia is not the spark, but it feeds the fire."
