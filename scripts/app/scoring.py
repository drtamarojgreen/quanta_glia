"""
This module provides higher-level functions to aggregate results from individual checks.
"""
from typing import Any, Dict, List, Tuple

from .evaluators import run_evaluation_point

def evaluate_answer(answer: str, evaluation_points: List[Dict[str, Any]]) -> Tuple[float, List[Dict[str, Any]]]:
    """
    Calculates a 0-1 score for a single answer against a list of evaluation points.

    Args:
        answer: The text answer to evaluate.
        evaluation_points: A list of evaluation point dictionaries.

    Returns:
        A tuple containing:
        - The final score (0.0 to 1.0).
        - A list of detailed results for each point.
    """
    if not evaluation_points:
        return 1.0, []

    total_points = len(evaluation_points)
    passed_points = 0
    details = []

    for point in evaluation_points:
        ok, note = run_evaluation_point(answer, point)
        if ok:
            passed_points += 1
        details.append({
            "point": point.get("text", "N/A"),
            "ok": ok,
            "note": note
        })

    score = passed_points / total_points if total_points > 0 else 1.0
    return score, details

def compare_answers(answers: List[str], evaluation_points_list: List[List[Dict[str, Any]]]) -> Tuple[int, List[Tuple[float, List[Dict[str, Any]]]]]:
    """
    Compares multiple answers and identifies the best-performing one.

    Args:
        answers: A list of text answers.
        evaluation_points_list: A list where each item is a list of evaluation points
                                corresponding to an answer in the `answers` list.

    Returns:
        A tuple containing:
        - The index of the winning answer.
        - A list of (score, details) tuples for each answer.
    """
    if not answers:
        return -1, []

    all_scores = [evaluate_answer(ans, points) for ans, points in zip(answers, evaluation_points_list)]

    scores_only = [s[0] for s in all_scores]
    winner_index = scores_only.index(max(scores_only)) if scores_only else -1

    return winner_index, all_scores