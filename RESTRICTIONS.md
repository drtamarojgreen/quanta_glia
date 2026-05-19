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
