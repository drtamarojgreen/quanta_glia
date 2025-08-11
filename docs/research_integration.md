# Research Integration Script

## Purpose

The `scripts/app/research_integration.py` script serves as a demonstration and entry point for a research integration and evaluation framework. It showcases how to use the scoring and evaluation modules to assess the quality and correctness of different text-based outputs.

The script provides several examples of how the evaluation framework can be used to compare and score answers for various research topics.

## Features Demonstrated

The script demonstrates the following evaluation capabilities through a series of examples:

### 1. Technical Explanation Comparison

This example evaluates and compares answers that provide a technical explanation. The evaluation is based on a set of criteria, including:

- **Keyword Presence**: Checks if the answer contains specific keywords.
- **Regex Matching**: Uses regular expressions to verify the presence of specific patterns, such as code blocks.
- **Length Check**: Ensures the answer meets a minimum length requirement.
- **Semantic Similarity**: Compares the semantic meaning of the answer to a canonical definition using embeddings.

### 2. JSON Schema Validation

This example demonstrates how to validate structured data. The evaluation checks if the output is:

- **Valid JSON**: Ensures the string is a correctly formatted JSON object.
- **Conforms to a Schema**: Validates the JSON object against a predefined schema, checking for required fields and correct data types.

### 3. Citation Checking

This example is designed to check for academic-style citations in a text. The evaluation criteria include:

- **Citation Format**: Uses regular expressions to find citations that match a specific pattern (e.g., `[n]`).
- **Minimum Count**: Ensures a minimum number of citations are present.
- **Keyword Presence**: Checks for the mention of key concepts related to the research topic.

### 4. Code Execution Validation

This example showcases the framework's ability to evaluate the correctness of code snippets. The evaluation involves:

- **Executing the Code**: Runs the provided code in a sandboxed environment.
- **Checking Stdout**: Verifies that the standard output of the script matches an expected string.
- **Checking Exit Code**: Ensures the script exits with a specific code (e.g., 0 for success).

Each of these examples illustrates a different facet of the evaluation framework, highlighting its flexibility in handling a wide range of assessment tasks.
