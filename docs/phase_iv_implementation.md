# Phase IV Development Plan: Research Integration & Automated Evaluation

## 1. Overview

This document outlines the development plan for `scripts/app/research_integration.py`. This module is a critical component for programmatically evaluating and comparing text-based outputs, likely from Large Language Models (LLMs), against a set of objective, configurable evaluation points.

The primary goal is to create a robust and extensible framework that can automate the quality assessment of generated content, enabling faster R&D cycles and objective model-to-model comparisons.

## 2. Core Architecture

The system is designed around three main concepts:

-   **Evaluation Points**: Atomic, verifiable checks against an answer. Each point has a `type`, human-readable `text`, and `params` for the specific check.
-   **Evaluators**: The `run_evaluation_point` function acts as a dispatcher, executing the correct logic based on the evaluation point's `type`.
-   **Scoring & Comparison**: The `evaluate_answer` function calculates a score (0-1) for a single answer, and `compare_answers` identifies the best-performing answer from a set.

## 3. Current Status (End of Phase III)

The initial implementation provides a solid foundation with several core features already in place.

### Implemented Validators:

-   `keyword`: Checks for the presence and minimum count of specified keywords.
-   `regex`: Validates the answer against a given regular expression pattern.
-   `length`: Verifies the word count of the answer is within a specified min/max range.

### Placeholder Validators:

The following validators are defined in the script but are not yet implemented. They represent the core work for Phase IV.

-   `embedding`: For semantic similarity checks.
-   `json_schema`: For validating JSON structure and content.
-   `code_test`: For executing and verifying code snippets.
-   `citation`: For checking the presence of citations.

## 4. Phase IV Development Roadmap

This phase focuses on implementing the placeholder validators, adding comprehensive testing, and hardening the module for production use.

### Task 1: Embedding Similarity Integration

-   **Objective**: Implement the `embedding` validator to enable semantic comparison between the generated answer and a target text.
-   **Action Items**:
    1.  Integrate the `sentence-transformers` library.
    2.  Select a pre-trained model suitable for semantic similarity (e.g., `all-MiniLM-L6-v2`).
    3.  Implement the `embedding_similarity` function to compute cosine similarity between two text embeddings.
    4.  Update `run_evaluation_point` to use the implemented function.
-   **Dependencies**: `sentence-transformers`, `torch`.

### Task 2: JSON Schema Validation

-   **Objective**: Implement the `json_schema` validator to ensure an answer contains valid JSON that conforms to a specified schema.
-   **Action Items**:
    1.  Integrate the `jsonschema` library.
    2.  Implement the validation logic within the `json_schema` block in `run_evaluation_point`.
    3.  The function should first safely parse the JSON from the answer string.
    4.  It should then use `jsonschema.validate` to check the parsed object against the schema provided in `params['schema']`.
    5.  Provide clear error messages on failure (e.g., "Invalid JSON" vs. "Schema validation failed: [error]").
-   **Dependencies**: `jsonschema`.

### Task 3: Secure Code Execution Sandbox

-   **Objective**: Implement the `code_test` validator to safely execute code snippets and check their output or success. **Security is paramount.**
-   **Action Items**:
    1.  **Research**: Investigate secure sandboxing solutions. The leading candidate is using **Docker containers** to isolate execution. Each test run would spin up a short-lived container, execute the code, and be torn down.
    2.  **Implementation**:
        -   Create a generic Dockerfile for a Python environment.
        -   The `code_test` logic will write the answer's code snippet to a temporary file.
        -   It will use the `docker` SDK for Python to run the container, mounting the script, and capturing `stdout`, `stderr`, and the exit code.
        -   The evaluation point's `params` will need to specify the expected output or success conditions.
    3.  **Security Hardening**: Ensure containers run with minimal privileges, no network access (unless required by the test), and have strict resource limits (CPU, memory, time).
-   **Dependencies**: `docker`.

### Task 4: Citation Verification

-   **Objective**: Implement the `citation` validator to check for the presence and format of citations.
-   **Action Items**:
    1.  **Define Scope**: For Phase IV, this will be a regex-based check to confirm the presence of citation markers (e.g., `[1]`, `[... ]`, `(Author, Year)`).
    2.  **Implementation**: The `params` will define a regex pattern for a valid citation and an expected minimum count.
    3.  **Future Work**: Note that true citation *verification* (checking if the source exists and supports the claim) is out of scope for Phase IV but is a potential future enhancement.

### Task 5: Comprehensive Unit Testing

-   **Objective**: Ensure the reliability and correctness of all validators and scoring logic.
-   **Action Items**:
    1.  Create a new test file: `tests/test_research_integration.py`.
    2.  Write unit tests for each validator type, covering both passing and failing cases.
    3.  Test edge cases: empty answers, malformed `evaluation_points`, etc.
    4.  Write tests for `evaluate_answer` and `compare_answers` to verify scoring and winner selection logic.
    5.  Integrate these tests into the main workspace test runner (`test_workspace.sh`) by ensuring `pytest` is the chosen test command for `quanta_glia`.

## 5. Testing Strategy

All new functionality will be developed with corresponding unit tests. The project's `test_workspace.sh` script will be used to ensure that changes to this module do not break other parts of the system and that the module itself is robust. The goal is to achieve >90% test coverage for `research_integration.py`.