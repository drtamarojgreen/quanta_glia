# Programming Vision

This document outlines a vision for a new suite of scripts dedicated to programmatic LLM integration and code generation. This suite, to be located in `scripts/app/`, will be named `programming_*` and will mirror the logic and structure of the existing `research_*` framework.

Where the `research_*` suite is designed to research topics and evaluate the quality of LLM-generated informational responses, the `programming_*` suite will be designed to **generate and evaluate LLM-written scripts**. It will provide a framework for using LLMs to automate the creation of new integration scripts and to validate their correctness.

## Core Components

The `programming_*` suite will consist of the following components, each analogous to its `research_*` counterpart:

*   **`programming_concepts.py`**: This script will be responsible for generating high-level goals for new scripts. Where `research_concepts.py` generates ideas for research (e.g., "Explain Quantum Computing"), `programming_concepts.py` will generate ideas for programs (e.g., "A script to summarize a text file using an LLM," "A script to convert a CSV file to JSON").

*   **`programming_examples.py`**: This file will contain a set of concrete programming tasks and corresponding evaluation criteria. Each example will include:
    *   A clear programming goal (e.g., "Write a Python function that takes a string and returns its sentiment").
    *   A set of evaluation points to programmatically verify the generated script. This could include running the script against test cases, checking for specific function signatures, or linting the code for style and errors.
    *   Example "correct" and "incorrect" generated scripts to test the evaluation framework itself.

*   **`programming_integration.py`**: This will be the main entry point for the framework. It will orchestrate the process of:
    1.  Taking a programming concept.
    2.  Using an LLM to generate a script that fulfills the concept.
    3.  Executing the evaluation points defined in `programming_examples.py` against the generated code.
    4.  Scoring the generated script based on its correctness and functionality.

*   **`programming_tools.py`**: This module will provide the essential tools for the framework, including:
    *   Functions to prompt an LLM specifically for code generation.
    *   A sandboxed environment to safely execute untrusted, LLM-generated code.
    *   Helpers to capture stdout/stderr, return values, and exceptions from the executed code for evaluation.

*   **`programming_semantics.py`**: Analogous to `research_semantics.py`, this script will be used for more advanced, context-aware code generation. It could be used to analyze an existing codebase to create a "semantic ontology" of it, which would allow the LLM to generate new scripts that are aware of the existing code's structure, conventions, and APIs.

*   **`programming_utils.py`**: A utility module for any helper functions and shared code used across the `programming_*` suite.

By developing this suite, we can create a powerful, self-improving system where LLMs not only provide information but also actively contribute to the development of the tools themselves.
