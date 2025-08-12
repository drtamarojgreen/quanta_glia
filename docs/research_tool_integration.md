# Research & Evaluation Framework Integration

## 1. Introduction

This document outlines how the Research & Evaluation Framework found in `scripts/app/` can be deeply integrated into the `QuantaGlia` workflow and the broader `Quanta Suite`. This framework provides a powerful, data-driven approach to content assessment, which can enhance and automate key processes in knowledge curation and governance.

## 2. Overview of the Research & Evaluation Framework

The framework, located in `scripts/app/`, is a modular system for programmatically evaluating text-based content. It operates on a set of "evaluation points," each representing an objective, verifiable check. Key features include:

- **A rich set of validators**: Including `keyword` matching, `regex` patterns, `JSON schema` validation, semantic `embedding` similarity, and safe `code_test` execution.
- **Aggregated Scoring**: The ability to combine results from multiple checks into a single, normalized quality score.
- **Extensibility**: A clear pattern for adding new, custom validators to meet evolving requirements.

By integrating this framework, we can replace manual or mocked assessments with automated, objective, and repeatable quality control.

## 3. Integration with `quanta_glia.py` (Knowledge Harvester)

The `quanta_glia.py` script is responsible for harvesting initial data from repositories. The evaluation framework can be integrated directly into this "ingestion" phase to provide an immediate quality filter and to enrich the harvested data.

### Proposed Workflow:

1.  **Harvest Content**: After `quanta_glia.py` clones a repository and extracts key files (e.g., `README.md`, `CONTRIBUTING.md`), the content is passed to the evaluation framework.
2.  **Define Evaluation Sets**: A predefined set of `evaluation_points` would be created for different file types.
    -   **For `README.md`**:
        -   Check for the presence of `keyword`s like "Installation", "Usage", and "Configuration".
        -   Use a `regex` check to ensure a "License" section exists.
        -   Check for a minimum `length` to avoid empty or stub files.
    -   **For `LICENSE`**:
        -   Use `keyword` checks to identify the license type (e.g., "MIT", "Apache License 2.0").
    -   **For `*.py` files**:
        -   Use the `code_test` validator to run linters or basic syntax checks.
        -   Use `regex` to check for the presence of docstrings.
3.  **Store Evaluation Score**: The resulting quality score and detailed results would be stored as metadata alongside the harvested files in the `knowledge_base`. This provides an immediate, queryable metric of repository quality.

This integration transforms the harvester from a simple data collector into a "quality-aware" ingestion engine.

### Implementation Details

The integration point for the evaluation framework within the harvester would be the `extract_key_info` function in `scripts/quanta_glia.py`.

1.  **Triggering Evaluation**: After the function reads the content of a target file (e.g., `README.md`), it would pass this content to an evaluation function, such as `evaluate_answer` from `scripts/app/scoring.py`.
2.  **Loading Rules**: The `evaluation_points` (the rules for the assessment) would be loaded from a new `evaluation` section in the `config.yaml` file. This allows for centralized and easily modifiable quality criteria.
3.  **Storing Results**: The output from the evaluation—an overall score and a detailed list of passed/failed checks—would be aggregated for all harvested files from the repository. This complete result set would then be saved as a new file, `evaluation_results.json`, in the repository's specific directory within the `knowledge_base`. This file acts as a persistent, machine-readable record of the repository's assessed quality at the time of ingestion.

## 4. Integration with `pruner.py` (Knowledge Pruner)

The `pruner.py` script currently relies on mocked scores for `usage_score`, `redundancy_score`, and `ethics_risk_score`. The evaluation framework is the perfect mechanism to replace these mocks with concrete, data-driven metrics.

### Replacing Mocked Scores:

-   **`usage_score`**: This can be directly calculated from the evaluation scores of a repository's documentation. A repository with a comprehensive, well-structured `README.md` is inherently more usable.
    -   **Implementation**: The pruner would read the stored evaluation metadata for a repo's `README.md` and normalize it to a 0-1 `usage_score`.

-   **`redundancy_score`**: The framework's `embedding` validator can be used to detect semantic overlap between repositories.
    -   **Implementation**: The pruner could create a representative text blob for each repository (e.g., by concatenating its README and key source files). The `embedding` validator would then be used to calculate the similarity score between a repository and all others in the `knowledge_base`. A high average similarity would result in a high `redundancy_score`.

-   **`ethics_risk_score`**: This can be calculated by scanning content for problematic terms or patterns.
    -   **Implementation**: A dedicated set of `evaluation_points` would define ethical checks. This could include:
        -   A `keyword` check for a list of non-inclusive terms.
        -   A `regex` check to find hardcoded secrets or API keys.
        The failure rate of these checks would directly translate into the `ethics_risk_score`.

This integration makes the pruning process far more intelligent and defensible, as decisions to `ARCHIVE` or `DELETE` would be based on objective data rather than just file age.

### Implementation Details

The integration point for the pruner is within the main loop of the `run_pruning` function in `scripts/pruner.py`, just after a `repo_record` is created.

1.  **Loading Scores**: For each repository being processed, the pruner would first check for the existence of the `evaluation_results.json` file in the repository's directory. If found, it would load the file and parse the scores.
2.  **Populating the `RepoRecord`**: The mocked scores in the `RepoRecord` dataclass would be replaced with actual data from the JSON file. For example, the `usage_score` would be set from the `overall_score` in the evaluation results. The `ethics_risk_score` would be derived from the failure rate of specific ethical checks defined in the rules.
3.  **Calculating Redundancy**: The `redundancy_score` is unique as it requires comparing multiple repositories. This would be calculated within the `run_pruning` loop. For each repository, the pruner would iterate through a sample of other repositories, using the `embedding` validator from `scripts.app.evaluators` to calculate semantic similarity. The average similarity would become the `redundancy_score`.
4.  **Informing Pruning Decisions**: The `calculate_composite_score` function would then be called with the fully populated, data-driven `RepoRecord`. The resulting score would lead to a much more accurate and evidence-based pruning decision (`KEEP`, `REVIEW`, `ARCHIVE`, or `DELETE`).

## 5. Integration with the Broader `Quanta Suite`

The evaluation framework is a general-purpose utility that can serve numerous components within the larger `Quanta Suite`.

-   **`QuantaEthos` (Ethical Governance)**: `QuantaEthos` could use the framework as its core enforcement mechanism. When another agent proposes an action (e.g., generating code, writing a document), `QuantaEthos` could use the framework to run a battery of ethical and safety checks on the content before approving it.

-   **`QuantaCogno` (Cognitive Modeling)**: If a cognitive modeling agent is tasked with generating text (e.g., writing a new README or summarizing a complex topic), it can use the framework for self-assessment. By evaluating its own output against a set of criteria, it can iterate and improve its response before finalizing it.

-   **`QuantaRetina` (Observation/Vision)**: If this component generates textual descriptions of visual data, the framework can be used to assess the quality and completeness of those descriptions. For example, ensuring that the description mentions key objects that were identified in the image.

-   **CI/CD and Automated Testing**: The evaluation framework can be integrated into the CI/CD pipeline (`test_workspace.sh`) to automatically assess the quality of documentation and other text-based artifacts during every build, preventing quality regressions.

## 6. Proposed Configuration (`config.yaml`)

To make the integration configurable, a new `evaluation` section should be added to the `config.yaml` file. This allows administrators to enable or disable the feature and to define the evaluation rules without modifying the scripts.

```yaml
evaluation:
  enabled: true
  rules:
    # Rules for evaluating README files
    readme:
      - text: "Contains an Installation section"
        type: "keyword"
        params": {"keywords": ["Installation", "Setup", "Install"], "min_count": 1}
      - text: "Contains a Usage section"
        type: "keyword"
        params": {"keywords": ["Usage", "Example", "Run"], "min_count": 1}

    # Rules for evaluating Python source files
    python:
      - text: "Contains a docstring"
        type: "regex"
        params": {"pattern": "^\\s*\\\"\\\"\\\""}

    # Rules for ethical scans on any file
    ethics:
      - text: "Does not contain placeholder credentials"
        type: "negation_keyword" # An example of an extended validator
        params": {"keywords": ["user@example.com", "your-api-key"]}
```

## 7. Data Format (`evaluation_results.json`)

The harvester needs to communicate the evaluation results to the pruner. A new file, `evaluation_results.json`, will be created in each repository's directory within the `knowledge_base`. This file serves as the data interchange format.

The proposed JSON schema is as follows:
```json
{
  "schema_version": "1.0",
  "evaluated_at": "2023-10-27T10:00:00Z",
  "overall_score": 0.82,
  "file_evaluations": {
    "README.md": {
      "score": 0.95,
      "results": [
        {
          "point": "Contains an Installation section",
          "ok": true,
          "note": "Keyword 'Installation' found"
        },
        {
          "point": "Contains a Usage section",
          "ok": true,
          "note": "Keyword 'Usage' found"
        }
      ]
    },
    "scripts/pruner.py": {
      "score": 0.70,
      "results": [
        {
          "point": "Contains a docstring",
          "ok": true,
          "note": "Regex pattern matched"
        }
      ]
    }
  }
}
```

This structure provides a high-level summary (`overall_score`) while also retaining the detailed results for each file, enabling fine-grained analysis and decision-making in the pruner.

## 8. Conclusion

The Research & Evaluation Framework is a pivotal tool that brings data-driven intelligence to the Quanta ecosystem. By integrating it into the `QuantaGlia` harvesting and pruning workflows, we can automate quality control and make more informed decisions about our knowledge assets. Its potential extends across the entire `Quanta Suite`, providing a unified mechanism for ensuring the quality, safety, and utility of any text-based content generated or managed by the system.
