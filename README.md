# QuantaGlia: Dynamic Knowledge Pruner

> **Disclaimer:** This project is in an early, aspirational stage. Most features are not yet implemented.
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Contributing](https://img.shields.io/badge/Contributing-welcome-brightgreen.svg)](CONTRIBUTING.md)
[![Security Policy](https://img.shields.io/badge/Security-Policy-blue.svg)](SECURITY.md)

## Current Status

QuantaGlia is a modular subsystem within the PrismQuanta framework designed to autonomously collect, curate, and evolve knowledge repositories.

> **Project State:** This project is in an early, functional stage. While the ambitious roadmap for intelligent pruning and spawning is still under development, the core component for information harvesting is operational.
>
> - **Implemented:**
>   - A "knowledge harvester" script (`scripts/quanta_glia.py`) that clones repositories and extracts key files.
>   - A test suite (`scripts/test_all.sh`) that validates the harvester script.
> - **Aspirational:**
>   - Fully autonomous pruning, spawning, and semantic annotation.
>   - Integration with other Quanta systems for learning and ethical governance.

---
## 🚀 Quickstart

The core functional component of this repository is an information-harvesting script, `scripts/quanta_glia.py`. This script can clone a remote Git repository or copy a local directory, and then extract key documents from it based on the configuration in `config.yaml`.

### Prerequisites

- Python 3.6+
- Git

### Usage

1.  **Clone this repository:**
    ```bash
    git clone https://github.com/drtamarojgreen/quanta_glia.git
    cd quanta_glia
    ```

2.  **Run the script:**
    You can provide a remote repository URL or a local directory path as an argument to the script. The extracted documents will be saved in the `knowledge_base` directory.

    *   **From a remote repository:**
        ```bash
        python3 scripts/quanta_glia.py https://github.com/drtamarojgreen/quanta_ethos.git
        ```

    *   **From a local directory:**
        ```bash
        python3 scripts/quanta_glia.py /path/to/your/local/repo
        ```

### Configuration

The script's behavior can be customized by editing the `config.yaml` file. You can specify which files and directories to target, where to cache cloned repositories, and where to store the extracted knowledge.

### Testing

A test suite is available to verify the script's functionality. To run the tests, execute the following command from the root of the repository:
```bash
./scripts/test_all.sh
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
> **Status:** Not yet implemented. This section describes a planned feature.

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

The behavior of the `quanta_glia.py` script is controlled by `config.yaml`. The script reads this file at runtime to determine which directories to use, what files to look for, and whether to enable features like summarization.

### Active Configuration

The following keys in `config.yaml` are currently in use:

```yaml
# Configuration for QuantaGlia System
main:
  knowledge_base: "./knowledge_base"
  repo_cache: "./repo_cache"
  target_topics:
    - "README"
    - "docs"
    - "ethics"
    - "usage"
  max_repos: 10

# Configuration for LLaMA.cpp integration
llamacpp:
  url: "http://localhost:8080/completion"
  enabled: false
```

### Future Configuration

The `pruning` section and other keys exist as placeholders for future development. An example of a more advanced future configuration is shown below:

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

## Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for more details on how to get started.

This project is governed by our [Code of Conduct](CODE_OF_CONDUCT.md).

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Final Thought

QuantaGlia is more than a tool—it's a neural helper, sustaining a logical and evolving digital ecosystem. When tuned with ethical priorities and thoughtful constraints, it ensures that only the most valuable knowledge survives and grows.

---

**Maintained by:** PrismQuanta Lab\
**Version:** 0.9.1 (Pre-Federated)

---

> "Glia is not the spark, but it feeds the fire."
