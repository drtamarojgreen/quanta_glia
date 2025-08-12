# Programming Vision

This document outlines a vision for a new suite of scripts dedicated to programmatic LLM integration and code generation. This suite, to be located in `scripts/app/`, will be named `programming_*` and will mirror the logic and structure of the existing `research_*` framework.

Where the `research_*` suite is designed to research topics and evaluate the quality of LLM-generated informational responses, the `programming_*` suite will be designed to **generate and evaluate LLM-written scripts**. It will provide a framework for using LLMs to automate the creation of new integration scripts and to validate their correctness.

## Core Components

The `programming_*` suite will consist of the following components, each analogous to its `research_*` counterpart:

*   **`programming_concepts.py`**: This script will be responsible for generating high-level goals for new scripts. Where `research_concepts.py` generates ideas for research topics (e.g., "Explain Quantum Computing"), `programming_concepts.py` will generate ideas for programs (e.g., "A script to summarize a text file using an LLM," "A script to convert a CSV file to JSON"). The output will be a list of programming challenges that can be fed into the integration script.

*   **`programming_examples.py`**: This file will contain a set of concrete programming tasks and corresponding evaluation criteria, mirroring the structure of `research_examples.py`. Each example will be a dictionary containing:
    *   A **`name`**: A descriptive name for the programming task (e.g., "File I/O and JSON Conversion").
    *   A **`programming_goal`**: A clear, concise description of the task (e.g., "Write a Python function that reads a CSV file from a given path and returns a JSON string of its contents").
    *   A list of **`evaluation_points`**: These are specific, verifiable criteria for success, analogous to those in `research_examples.py`. For programming, these will focus on correctness and code quality. Examples include:
        *   `{"type": "unit_test", "params": {"input": "file.csv", "expected_output": "[...]"}}`
        *   `{"type": "linting", "params": {"linter": "pylint", "min_score": 8.0}}`
        *   `{"type": "signature_check", "params": {"function_name": "csv_to_json", "args": ["file_path"]}}`
        *   `{"type": "exception_handling", "params": {"input": "non_existent_file.csv", "expects_exception": "FileNotFoundError"}}`
    *   A list of **`example_scripts`**: Sample LLM-generated scripts, including both "correct" and "incorrect" versions, to test the evaluation framework itself.
    *   An **`evaluation_type`**: Either `individual` (each script is scored independently) or `comparison` (scripts are scored and ranked against each other).

*   **`programming_integration.py`**: This will be the main entry point for the framework, orchestrating the end-to-end process. It will be responsible for:
    1.  Taking a programming concept from `programming_concepts.py` or a specific example from `programming_examples.py`.
    2.  Using an LLM to generate a script that fulfills the programming goal.
    3.  Executing the defined `evaluation_points` against the generated code in a secure, sandboxed environment.
    4.  Scoring the generated script based on the results of the evaluation, producing a final score and a detailed report.

*   **`programming_tools.py`**: This module will provide the essential, specialized tools for the framework. This is analogous to `research_tools.py` but focused on code. It will include:
    *   Functions to construct prompts specifically engineered for code generation, potentially including techniques like few-shot prompting with examples from `programming_examples.py`.
    *   A robust, sandboxed environment to safely execute untrusted, LLM-generated code (e.g., using Docker containers or `subprocess` with restricted permissions).
    *   Helper functions to interface with the sandbox, capture stdout/stderr, return values, and exceptions, and measure resource usage (e.g., execution time, memory).

*   **`programming_semantics.py`**: Analogous to `research_semantics.py`, this script will be used for more advanced, context-aware code generation. It could be used to analyze an existing codebase to create a "semantic ontology" of it. This ontology would allow the LLM to generate new scripts that are aware of the existing code's structure, conventions, APIs, and even its business logic, leading to more integrated and useful code.

*   **`programming_utils.py`**: A utility module for any helper functions and shared code used across the `programming_*` suite, such as file handling, logging configuration, or common data structures.

## Detailed Workflow

The framework can be used in two primary modes: **concept-driven generation** and **example-driven evaluation**.

### 1. Concept-Driven Generation

This mode is for open-ended script generation and discovery.

1.  **Concept Generation**: An operator runs `programming_concepts.py` to generate a list of potential programming tasks (e.g., "A script to scrape a website for links").
2.  **Integration Run**: The operator selects a concept and passes it to `programming_integration.py`.
3.  **LLM Prompting**: The integration script uses `programming_tools.py` to construct a detailed prompt for the LLM, asking it to generate a script to solve the task.
4.  **Code Generation**: The LLM returns a block of code.
5.  **Automated Evaluation**: The integration script automatically generates a suite of basic tests (e.g., does it run without syntax errors? Does it produce any output?). This provides a baseline level of quality control.
6.  **Scoring and Reporting**: The script is scored based on the automated tests. The generated script, its score, and a report are saved for review.

### 2. Example-Driven Evaluation

This mode is for rigorously testing LLM capabilities against a predefined set of benchmarks.

1.  **Define Example**: A developer adds a new, well-defined programming challenge to `programming_examples.py`, including a clear goal, a list of precise `evaluation_points` (unit tests, performance metrics, etc.), and reference scripts.
2.  **Integration Run**: An operator runs `programming_integration.py` with the `--run-examples` flag, specifying one or more examples to test.
3.  **LLM Prompting**: For each example, the integration script prompts the LLM to generate a script that solves the `programming_goal`.
4.  **Rigorous Evaluation**: The generated script is executed against the full suite of evaluation points defined in the example. This could involve running dozens of unit tests, checking for specific edge case handling, and linting for code quality.
5.  **Scoring and Comparison**: The script is given a detailed score based on its performance. If run in `comparison` mode, the outputs of multiple LLMs or prompt strategies can be benchmarked against each other to find the optimal approach.

## Evaluation and Scoring

The evaluation framework is the core of the `programming_*` suite. It is designed to be both robust and flexible, allowing for a wide range of correctness checks. The `evaluate_script` function (a conceptual equivalent to `evaluate_answer` in the research suite) will take a generated script and a list of evaluation points and return a score and a detailed report.

Key evaluation types will include:

*   **Unit Testing**: The most critical evaluation type. This involves running the generated code against a predefined set of inputs and asserting that the output is correct. This is essential for verifying the functional correctness of the code.
*   **Linting and Static Analysis**: The code will be analyzed for style, errors, and code smells using standard linters (e.g., Pylint, Flake8 for Python). This ensures that the generated code is not only functional but also readable and maintainable.
*   **Function Signature Verification**: This check ensures that the generated code contains functions with the expected names and parameters. This is useful for tasks that require a specific API to be implemented.
*   **Exception Handling**: This tests the code's robustness by providing invalid inputs or simulating error conditions and verifying that the code handles them gracefully (e.g., by raising the expected exceptions).
*   **Resource Usage**: The framework will measure the execution time and memory consumption of the generated code. This can be used to evaluate the efficiency of the code and to prevent runaway processes.
*   **Semantic Correctness**: For more advanced use cases, the framework could use techniques like semantic similarity to compare the generated code to a reference implementation, or to check if the code's logic aligns with a natural language description of the task.

The final score will be a weighted average of the results of these evaluation points, providing a single, quantitative measure of the LLM's programming ability.

## Synergy with `research_*`

While the `programming_*` suite is designed to be a standalone framework, it can also be used in conjunction with the `research_*` suite to create a more powerful and comprehensive system. For example:

*   **From Research to Code**: `research_concepts.py` could generate a high-level research topic (e.g., "Analyze the sentiment of customer reviews"). The output of this could then be used as an input to `programming_concepts.py` to generate a specific programming task (e.g., "Write a Python script that takes a text file of customer reviews and outputs a CSV with the sentiment of each review").
*   **Code-Aware Research**: The `research_*` suite could use the `programming_*` suite to generate code examples to include in its informational responses. For instance, when asked to explain a programming concept, the `research_integration.py` script could call upon `programming_integration.py` to generate a correct, working code sample.
*   **Self-Improving Systems**: The two suites could be used together to create a system that can both write and reason about its own code. The `programming_*` suite could generate a new script, and the `research_*` suite could be used to evaluate the script's documentation, comments, and overall clarity.

By developing this suite, we can create a powerful, self-improving system where LLMs not only provide information but also actively contribute to the development of the tools themselves.
