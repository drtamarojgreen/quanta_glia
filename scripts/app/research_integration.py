"""
Main entry point for the research integration and evaluation framework.
This script demonstrates how to use the evaluation and scoring modules.
"""
import logging
import sys
import argparse
from pathlib import Path

# Add project root to the Python path to allow for absolute imports
# This is necessary to run the script directly
project_root = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(project_root))

from scripts.app.scoring import evaluate_answer, compare_answers
from scripts.app.research_tools import (
    connect_to_llm,
    create_research_topic,
    create_evaluation_points,
)
# Conditionally import examples to avoid errors if the file is missing
try:
    from scripts.app.research_examples import EXAMPLES
except ImportError:
    EXAMPLES = None


def run_single_topic_evaluation(concept: str, llm_client):
    """
    Runs a full research and evaluation cycle:
    1. Defines a concept.
    2. Creates a research topic.
    3. Generates evaluation points.
    4. Gets an answer from the configured LLM.
    5. Evaluates the answer and prints a report.
    """
    topic = create_research_topic(concept)
    print(f"--- Research Integration Run ---")
    print(f"Research Topic: {topic}\n")

    # 3. Generate evaluation points for the topic
    evaluation_points = create_evaluation_points(topic)
    print("Generated Evaluation Points:")
    for point in evaluation_points:
        print(f"  - {point['text']} (type: {point['type']})")
    print("-" * 20 + "\n")

    # 4. Get an answer from the LLM
    answer = llm_client.get_answer(topic)
    print(f"LLM Answer:\n---\n{answer}\n---\n")

    # 5. Evaluate the answer
    print("Evaluating Answer...")
    score, details = evaluate_answer(answer, evaluation_points)
    print(f"\nFinal Score: {score:.2f}")
    print("Evaluation Details:")
    for d in details:
        status = 'PASS' if d['ok'] else 'FAIL'
        print(f"  - {status}: {d['point']} ({d['note']})")


def run_example_set(example: dict):
    """
    Runs an evaluation for a single example set from research_examples.
    This function handles both 'comparison' and 'individual' evaluation types.
    """
    print(f"--- Running Example: {example['name']} ---")
    print(f"Research Topic: {example['research_topic']}")

    evaluation_points = example['evaluation_points']
    answers = example['answers']

    if example['evaluation_type'] == 'comparison':
        print(f"Comparing {len(answers)} pre-defined answers...")

        winner_idx, score_details = compare_answers(
            answers,
            [evaluation_points] * len(answers)
        )
        print(f"\nBest answer: Model {winner_idx + 1}")
        for idx, (score, details) in enumerate(score_details):
            print(f"  Model {idx + 1} score: {score:.2f}")
            for d in details:
                status = 'PASS' if d['ok'] else 'FAIL'
                print(f"    - {status}: {d['point']} ({d['note']})")

    elif example['evaluation_type'] == 'individual':
        print(f"Evaluating {len(answers)} pre-defined answers individually...")
        for i, ans in enumerate(answers):
            print(f"\n--- Evaluating Answer {i+1} ---")
            if len(ans) > 100:
                print(f"Answer Snippet: {ans[:100]}...")
            else:
                print(f"Answer: {ans}")

            score, details = evaluate_answer(ans, evaluation_points)
            print(f"Score: {score:.2f}")
            for d in details:
                status = 'PASS' if d['ok'] else 'FAIL'
                print(f"  - {status}: {d['point']} ({d['note']})")

    print("-" * 20 + "\n")


def main():
    """
    Main entry point. Parses command-line arguments to determine which evaluation to run.
    """
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    parser = argparse.ArgumentParser(
        description="Run research integration and evaluation tasks.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "--concept",
        type=str,
        help="Run a single evaluation for the given research concept using the LLM."
    )
    if EXAMPLES is not None:
        parser.add_argument(
            "--run-examples",
            action="store_true",
            help="Run all predefined examples from research_examples.py.\n(Uses pre-canned answers, does not call the LLM)."
        )

    args = parser.parse_args()

    if args.run_examples:
        print("--- Running All Predefined Examples ---")
        for example in EXAMPLES:
            run_example_set(example)
    else:
        llm_client = connect_to_llm()
        concept_to_run = args.concept if args.concept else "TissLang"
        run_single_topic_evaluation(concept_to_run, llm_client)

    print("\n" + "-" * 20)
    print("Run complete.")


if __name__ == "__main__":
    main()
