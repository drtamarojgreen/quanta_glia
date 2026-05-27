# QuantaGlia System Structural Restrictions (C++ Implementation)

## General
- REQUIRE all Phase III enhancements to be implemented in C++.
- REQUIRE all functionality to be available via the `sorrel` console terminal application.
- REQUIRE explicit verification of all empirical outputs.

## Reporting Enhancements
- FORBID the usage of external NLP or static analysis libraries for tech stack identification; use extension frequency counting.
- REQUIRE ISO 8601 UTC timestamps in all report records.
- LIMIT CSV modifications to the addition of `tech_stack` and `timestamp` columns.

## Harvester Enhancements
- LIMIT directory scanning in Harvester to configured `search_paths` and `max_depth`.
- REQUIRE Harvester to respect `max_lines_per_file` when extracting content to prevent resource exhaustion.
- REQUIRE logging verbosity to be controlled via configuration.

## Maintenance (Pruner) Enhancements
- FORBID destructive actions (ARCHIVE, DELETE) on any repository identified as an active dependency.
- REQUIRE the Pruner to build a dependency map by scanning for `requirements.txt`, `package.json`, `go.mod`, and `pom.xml` before evaluating repositories.
- LIMIT dependency identification to exact matches of repository names within manifest files.

## Developer Workflow Enhancements
- REQUIRE `workspace-status` to use `glia::cli::Terminal::printTable` to ensure a consistent, aligned terminal experience.
- FORBID `workspace-sync` from performing non-rebase pulls or any operation that causes merge conflicts automatically.
- REQUIRE `quick-commit` to stage all modified files (`git add .`) before executing the commit command.
- LIMIT `glia` workspace commands to repositories located within the `workspace/` directory relative to the current working directory.

## Modularity and Data Integrity
- REQUIRE all `glia` commands to be defined in `/rules/rules.xml`.
- FORBID manual command registration in `main.cpp`; registry must use the automated `CommandLoader`.
- REQUIRE XML schema validation against `/rules/rules.xsd` before registry population.

## Scoring and Structural Integrity
- REQUIRE a 10-point penalty for each SDD test card using "lazy" binary logic (1/0) instead of descriptive numeric metrics (e.g., scores 0-100).
- FORBID "fallback logic" in source code, requiring robust error propagation instead.
- REQUIRE the scoring command to apply these penalties to the structural Health Index empirically.
- FORBID the use of raw pointers (`new`/`delete`) and empty catch blocks across the repository.

## TUI Enhancements
- REQUIRE all TUI enhancements to utilize smart pointers (`std::unique_ptr`, `std::shared_ptr`) for resource management; FORBID raw pointers.
- FORBID the usage of external TUI libraries beyond standard C++ and Qt components.
- REQUIRE fuzzy matching logic to be implemented natively within the toolkit utilities.
