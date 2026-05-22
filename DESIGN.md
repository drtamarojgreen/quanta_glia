# QuantaGlia System Enhancement Design (C++ Implementation)

## Architectural Vision
This document outlines the design for the Phase III enhancements of the QuantaGlia system, focusing on Maintenance (Pruner), Discovery (Harvester), and Analytics (Reporting) modules. Following a system-wide directive, all enhancements are implemented natively in C++ and integrated into the `sorrel` standalone console application, ensuring high-performance structural enforcement and empirical verification via SDD.

## 1. Reporting & Analytics Enhancements (C++)
The reporting module will be evolved from basic file counting to deeper codebase insights.
- **Tech Stack Identification:** Instead of just listing file counts, the system will identify the primary programming language/technology of a repository by analyzing the frequency of source file extensions.
- **Temporal Traceability:** Every report entry will include a UTC ISO 8601 timestamp to track the knowledge base state over time.
- **Enhanced Data Schema:** The CSV output will be expanded to include `tech_stack` and `timestamp` fields.

## 2. Harvester Enhancements (C++)
The Harvester will be made more robust and configurable for diverse environments.
- **Selective Extraction:** Users can now limit the scope of information gathering by defining `search_paths` and `max_depth`. This prevents inefficient scans of large repositories.
- **Resource Management:** A `max_lines_per_file` constraint will be introduced to ensure the knowledge base remains distilled and avoids capturing excessive boilerplate.
- **Observability:** Configurable `log_level` will allow users to control the verbosity of the harvesting process.

## 3. Maintenance (Pruner) Enhancements (C++)
The Pruner will transition from simple age-based decisions to dependency-aware maintenance.
- **Dependency Graph Awareness:** Before any destructive action (ARCHIVE or DELETE), the Pruner will build a dependency map by scanning repositories for common dependency manifests (`requirements.txt`, `package.json`, `go.mod`, `pom.xml`).
- **Active Protection:** Repositories identified as dependencies of other projects in the knowledge base will be strictly protected from pruning, regardless of their age or usage scores.

## 4. Developer Workflow Enhancements (C++)
To improve developer productivity, `glia` will include high-level workspace management commands.
- **Workspace Status (`workspace-status`):** Iterates through all repositories in the `workspace/` directory and displays their current branch, sync status (ahead/behind), and if they have uncommitted changes.
- **Workspace Sync (`workspace-sync`):** Synchronizes all repositories in the `workspace/` directory by fetching updates and rebasing local changes, ensuring the developer is always working on the latest state.
- **Quick Commit (`quick-commit`):** A streamlined command to stage all modified files and commit them with a single message, reducing the friction of frequent commits during development.

## 5. Modular XML Command Registration
To reduce code repetition and improve system modularity, `glia` transitions to a data-driven command registration model.
- **Rule-Based Definitions:** All commands must be defined in `/rules/rules.xml` and validated against `/rules/rules.xsd`.
- **Dynamic Loading:** The `CommandRegistry` uses a `CommandLoader` module (based on `QtXml`) to discover and initialize commands at runtime.
- **Metadata Centralization:** Command names, descriptions, and execution targets are managed centrally in XML, allowing for rapid expansion and easier auditing.

## Implementation Strategy
Implementation will proceed through the SDD lifecycle:
1. **Restrictions:** Define formal guardrails in `RESTRICTIONS.md`.
2. **Coding:** Implement core logic in C++ within the `src/` directory, integrated into the `sorrel` binary.
3. **Verification:** Empirically verify C++ logic using SDD test cards and the standalone console interface.
