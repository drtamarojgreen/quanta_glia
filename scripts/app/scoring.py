"""
This module provides higher-level functions to aggregate results from individual checks.
"""
from typing import Any, Dict, List, Tuple

from .evaluators import run_evaluation_point

def evaluate_answer(answer: str, evaluation_points: List[Dict[str, Any]]) -> Tuple[float, Dict[str, float], List[Dict[str, Any]]]:
    """
    Calculates scores for a single answer against a list of evaluation points.
    This now supports weighted scoring and categorical aggregation.

    Args:
        answer: The text answer to evaluate.
        evaluation_points: A list of evaluation point dictionaries.

    Returns:
        A tuple containing:
        - The final weighted score (0.0 to 1.0).
        - A dictionary of scores for each category.
        - A list of detailed results for each point.
    """
    if not evaluation_points:
        return 1.0, {}, []

    total_weight = 0.0
    passed_weight = 0.0
    category_totals: Dict[str, float] = {}
    category_passed: Dict[str, float] = {}
    details = []

    for point in evaluation_points:
        weight = float(point.get("weight", 1.0))
        category = point.get("category", "General")

        ok, note, evidence = run_evaluation_point(answer, point)

        total_weight += weight
        category_totals[category] = category_totals.get(category, 0.0) + weight

        if ok:
            passed_weight += weight
            category_passed[category] = category_passed.get(category, 0.0) + weight

        details.append({
            "point": point.get("text", "N/A"),
            "category": category,
            "ok": ok,
            "note": note,
            "evidence": evidence
        })

    # Calculate final weighted score
    score = passed_weight / total_weight if total_weight > 0 else 1.0

    # Calculate category scores
    category_scores = {
        cat: category_passed.get(cat, 0.0) / total
        for cat, total in category_totals.items()
    }

    return score, category_scores, details

def compare_answers(answers: List[str], evaluation_points_list: List[List[Dict[str, Any]]]) -> Tuple[int, List[Tuple[float, Dict[str, float], List[Dict[str, Any]]]]]:
    """
    Compares multiple answers and identifies the best-performing one.

    Args:
        answers: A list of text answers.
        evaluation_points_list: A list where each item is a list of evaluation points
                                corresponding to an answer in the `answers` list.

    Returns:
        A tuple containing:
        - The index of the winning answer.
        - A list of (score, category_scores, details) tuples for each answer.
    """
    if not answers:
        return -1, []

    all_scores = [evaluate_answer(ans, points) for ans, points in zip(answers, evaluation_points_list)]

    if not all_scores:
        return -1, []

    scores_only = [s[0] for s in all_scores]
    winner_index = scores_only.index(max(scores_only)) if scores_only else -1

    return winner_index, all_scores