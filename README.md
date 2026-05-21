# QuantaGlia: Dynamic Knowledge Pruner

> **Disclaimer:** This README describes the aspirational goals and architecture of the QuantaGlia project. The current implementation is in an early stage. Key foundational pieces like the multi-repository **bootstrapper** (`scripts/bootstrap.sh`) and an initial version of the **pruner** (`scripts/pruner.py`) are functional. However, many advanced features described below (such as repository spawning and semantic annotation) are not yet implemented.

---

## 🛠️ Current Implementation: Glia C++ CLI (v1.1.0)

QuantaGlia is now a production-grade, self-contained automation engine and developer utility.

### 🧠 Standalone & Global Architecture
- **Zero-Dependency Portability**: The `glia` binary embeds its own default configuration (`rules.xml` and `rules.xsd`).
- **Dynamic Initialization**: Running `glia init` creates a global `~/.glia` directory and populates it with these rules directly from the binary, ensuring it works in any directory and on any machine.
- **Data-Driven Logic**: All automation workflows are defined in XML. You can add, modify, or extend the tool's capabilities by editing `~/.glia/rules.xml` without recompiling the source code.
- **XSD Validation**: All command definitions are validated against a structural schema to ensure reliability.

### ⚙️ Advanced Automation Engine
- **Multi-Step Sequences**: Supports complex workflows defined as a sequence of dependent steps. The engine halts immediately if any step fails, preventing corrupted states.
- **Complex Command Integration**: seamlessly bridges XML-defined wrappers with specialized C++ logic for interactive or filesystem-heavy operations.
- **Operational Lifecycle**: Built-in `--version` and `--update` flags, the latter providing a real Git-based migration path for the tool itself.

### 🖥️ Interactive TUI (Node-Graph Editor)
Launch the TUI with `glia tui` for a visual automation experience:
- **Node-Graph Workspace**: Visualize your workflows and dependencies in a 2D graph.
- **Sidebar Command Menu**: Quick access and discovery of all registered tools.
- **Keyboard Navigation**: Fully interactive, character-at-a-time control using standard terminal ANSI codes (no external UI libraries required).

### 📋 Comprehensive Command Reference

#### Core Management
- `init`: Global setup of `~/.glia` and binary PATH integration.
- `tui`: Interactive workflow and node-graph editor.
- `report-status`: Real-time inventory of commands and execution of the `glia_unit_tests` suite.

#### Git Automation
- `checkout-latest`: Smart branch detection and checkout.
- `commit-changes`: Interactive staging and committing with real-time feedback.
- `update-repo`: Safe, multi-stage fetch-rebase-push pipeline.
- `git-find-commit`: Pattern-based commit history search.
- `git-cleanup-local`: Interactively delete merged local branches.

#### Environment & Build
- `env-rebuild`: Multi-step sequence to tear down, rebuild, and verify the Docker environment.
- `env-doctor`: Full diagnostic of Docker, Git, and CMake environments.
- `build-lint-cmake`: Static analysis of build configurations.
- `test-unit`: Execution of the project's ctest suite.
- `env-shell`: Interactive shell access to containerized services.
- `env-clean-volumes`: Safe removal of unused Docker volumes.

#### Quality, Documentation & Project
- `lint-check-format`: Code style verification.
- `lint-security` / `lint-secrets`: Static security analysis and credential scanning.
- `doc-todo-list`: Extraction of TODOs/FIXMEs into a markdown report.
- `doc-update-changelog`: Automatic generation of project history.
- `proj-estimate`: Algorithmic estimation of project size and complexity.
- `sys-alias-setup`: Automatic configuration of shell aliases (`g`, `gc`, `gu`).

---

## 🚀 Workspace Setup and Testing

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
