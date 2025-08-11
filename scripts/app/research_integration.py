"""
Main entry point for the research integration and evaluation framework.
This script demonstrates how to use the evaluation and scoring modules.
"""
import logging
from .scoring import evaluate_answer, compare_answers
from .research_examples import EXAMPLES


def run_evaluation(example):
    """
    Runs an evaluation for a single example.

    Args:
        example (dict): A dictionary containing the research topic, evaluation points,
                        answers, and evaluation type.
    """
    print(f"--- {example['name']} ---")
    print(f"Research Topic: {example['research_topic']}")

    if example['evaluation_type'] == 'comparison':
        winner, score_details = compare_answers(
            example['answers'],
            [example['evaluation_points']] * len(example['answers'])
        )
        print(f"Best answer: Model {winner + 1}")
        for idx, (score, details) in enumerate(score_details):
            print(f"Model {idx + 1} score: {score:.2f}")
            for d in details:
                print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")

    elif example['evaluation_type'] == 'individual':
        for i, ans in enumerate(example['answers']):
            print(f"\nEvaluating Answer {i+1}:")
            if len(ans) < 100:
                print(ans)
            else:
                print(ans[:100] + "...")

            score, details = evaluate_answer(ans, example['evaluation_points'])
            print(f"Score: {score:.2f}")
            for d in details:
                print(f"  - {'PASS' if d['ok'] else 'FAIL'}: {d['point']} ({d['note']})")

    print("-" * 20 + "\n")


def main():
    """Runs the example evaluations."""
    # Configure logging to show info-level messages
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

    for example in EXAMPLES:
        run_evaluation(example)


if __name__ == "__main__":
    main()
