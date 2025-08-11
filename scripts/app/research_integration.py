"""
Main entry point for the research integration and evaluation framework.
This script demonstrates how to use the evaluation and scoring modules.
"""
import logging
from .scoring import evaluate_answer, compare_answers


def main():
    """Runs the example evaluations."""
    # Configure logging to show info-level messages
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    # --- Example 1: Comparing answers for a technical explanation ---
    print("--- Example 1: Technical Explanation ---")
    research_topic_1 = "Explain the concept of TissLang and give two code examples."

    evaluation_points_1 = [
        {"text": "The answer clearly defines TissLang's purpose.",
         "type": "keyword",
         "params": {"keywords": ["TissLang", "purpose", "language"], "min_count": 3}},
        {"text": "The answer provides at least two distinct code examples.",
         "type": "regex",
         "params": {"pattern": r"```(python|tisslang).*?```.*?```(python|tisslang).*?```"}},
        {"text": "The answer is at least 50 words long.",
         "type": "length",
         "params": {"min": 50}},
        {"text": "The explanation is semantically similar to the canonical definition.",
         "type": "embedding",
         "params": {"target": "TissLang is a domain-specific language for defining tissue simulations.", "threshold": 0.7}}
    ]

    answers_1 = [
        # Model 1 (Good, but brief)
        "TissLang is a special language for tissue simulation. Here is one example: ```python\nprint('hello')\n``` and another ```python\nprint('world')\n```. Its purpose is clear.",
        # Model 2 (Better, more detailed and semantically closer)
        "TissLang is a domain-specific language designed from the ground up for defining complex tissue simulations and cellular interactions. The purpose of the language is to provide a clear and concise syntax. Here are two code blocks: ```tisslang\ninit_grid(10,10)\n``` and ```tisslang\nadd_cells(5)\n```"
    ]

    winner_1, score_details_1 = compare_answers(answers_1, [evaluation_points_1] * len(answers_1))
    print(f"Research Topic: {research_topic_1}")
    print(f"Best answer: Model {winner_1 + 1}")
    for idx, (score, details) in enumerate(score_details_1):
        print(f"Model {idx + 1} score: {score:.2f}")
        for d in details:
            print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")
    print("-" * 20)

    # --- Example 2: Checking for structured JSON output ---
    print("\n--- Example 2: JSON Schema Validation ---")
    research_topic_2 = "Provide user data in JSON format for user_id 123."
    user_schema = {
        "type": "object",
        "properties": {
            "user_id": {"type": "number"},
            "user_name": {"type": "string"},
            "is_active": {"type": "boolean"},
        },
        "required": ["user_id", "user_name", "is_active"]
    }

    evaluation_points_2 = [
        {"text": "The output is valid JSON that conforms to the user schema.",
         "type": "json_schema",
         "params": {"schema": user_schema}},
    ]

    answers_2 = [
        # Model 1 (Invalid JSON - single quotes)
        "{'user_id': 123, 'user_name': 'test', 'is_active': true}",
        # Model 2 (Valid JSON, but fails schema - missing required field)
        '{"user_id": 123, "user_name": "test"}',
        # Model 3 (Correct)
        '{"user_id": 123, "user_name": "test", "is_active": true}'
    ]

    for i, ans in enumerate(answers_2):
        print(f"\nEvaluating Answer {i+1}:")
        print(ans)
        score, details = evaluate_answer(ans, evaluation_points_2)
        for d in details:
            print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")

    print("\n" + "-" * 20)

    # --- Example 3: Checking for citations ---
    print("\n--- Example 3: Citation Checking ---")
    research_topic_3 = "Summarize the findings of the paper 'Attention is All You Need' and cite your sources."

    evaluation_points_3 = [
        {"text": "The answer contains at least 2 citations in the format [n].",
         "type": "citation",
         "params": {"pattern": r'\[\d+\]', "min_count": 2}},
        {"text": "The answer mentions the 'Transformer' model.",
         "type": "keyword",
         "params": {"keywords": ["Transformer"], "min_count": 1}}
    ]

    answers_3 = [
        # Model 1 (Fails, no citations)
        "The paper introduced the Transformer model, which is very important.",
        # Model 2 (Fails, only one citation)
        "The paper 'Attention is All You Need' introduced the Transformer model [1]. It relies on self-attention mechanisms.",
        # Model 3 (Passes)
        "The paper introduced the Transformer model, a novel network architecture based solely on attention mechanisms [1]. It dispenses with recurrence and convolutions entirely [2]."
    ]

    winner_3, score_details_3 = compare_answers(answers_3, [evaluation_points_3] * len(answers_3))
    print(f"Research Topic: {research_topic_3}")
    print(f"Best answer: Model {winner_3 + 1}")
    for idx, (score, details) in enumerate(score_details_3):
        print(f"Model {idx + 1} score: {score:.2f}")
        for d in details:
            print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")
    print("-" * 20)

    # --- Example 4: Checking for executable code output ---
    print("\n--- Example 4: Code Execution Validation ---")
    research_topic_4 = "Write a Python script that prints 'Hello, World!'."

    evaluation_points_4 = [
        {"text": "The script should print 'Hello, World!' to stdout.",
         "type": "code_test",
         "params": {"stdout": "Hello, World!"}},
        {"text": "The script should exit with code 0.",
         "type": "code_test",
         "params": {"exit_code": 0}}
    ]

    answers_4 = [
        # Model 1 (Correct)
        "Here is the script:\n```python\nprint('Hello, World!')\n```",
        # Model 2 (Incorrect output)
        "Of course:\n```python\nprint('Hello World')\n```",
        # Model 3 (Runtime error)
        "Certainly:\n```python\nprint(hello)\n```"
    ]

    for i, ans in enumerate(answers_4):
        print(f"\nEvaluating Answer {i+1}:")
        score, details = evaluate_answer(ans, evaluation_points_4)
        print(f"Score: {score:.2f}")
        for d in details:
            print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")
    print("-" * 20)


if __name__ == "__main__":
    main()
