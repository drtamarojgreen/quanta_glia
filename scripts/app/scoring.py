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
    results_by_id: Dict[str, bool] = {}

    for point in evaluation_points:
        point_id = point.get("id")
        depends_on = point.get("depends_on")
        weight = float(point.get("weight", 1.0))
        is_informational = point.get("informational", False)
        category = point.get("category", "General")

        # Check for dependencies
        dependency_failed = False
        if depends_on:
            if depends_on not in results_by_id:
                # This case implies an evaluation point depends on one that hasn't run yet.
                # A more robust implementation might pre-sort or build a dependency graph.
                # For now, we'll treat it as a failure to be safe.
                dependency_failed = True
                note = f"Skipped: Dependency '{depends_on}' not found or has not run yet."
            elif not results_by_id[depends_on]:
                dependency_failed = True
                note = f"Skipped: Dependency '{depends_on}' failed."

        if not dependency_failed:
            ok, note, evidence, duration = run_evaluation_point(answer, point)
        else:
            # If dependency failed, the check is not run.
            ok, evidence, duration = False, None, 0.0

        # Informational checks do not contribute to the score.
        if not is_informational:
            total_weight += weight
            category_totals[category] = category_totals.get(category, 0.0) + weight
            if ok:
                passed_weight += weight
                category_passed[category] = category_passed.get(category, 0.0) + weight

        # Store result if it has an ID for other points to depend on
        if point_id:
            results_by_id[point_id] = ok

        details.append({
            "point": point.get("text", "N/A"),
            "id": point_id,
            "category": category,
            "informational": is_informational,
            "ok": ok,
            "note": note,
            "evidence": evidence,
            "duration_ms": duration
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