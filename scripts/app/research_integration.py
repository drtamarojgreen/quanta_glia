import re
import json
from typing import Dict, Any, List, Tuple
from collections import defaultdict
# optional: from sentence_transformers import SentenceTransformer, util

# --- Placeholder embedding similarity function ---
def embedding_similarity(a: str, b: str) -> float:
    # TODO: Integrate real embedding model (e.g., model.encode + cosine similarity)
    return 0.0


# --- Core validator execution ---
def run_evaluation_point(answer: str, point: Dict[str, Any]) -> Tuple[bool, str]:
    """
    Evaluate an answer against a single evaluation point.

    Each evaluation point is atomic and confirmable, so this function returns
    (pass/fail, diagnostic_note).
    """
    t = point['type']
    p = point.get('params', {})

    if t == 'keyword':
        kws = p['keywords']
        count = sum(1 for k in kws if re.search(r'\b' + re.escape(k) + r'\b', answer, re.I))
        return count >= p.get('min_count', 1), f"found {count}/{len(kws)}"

    if t == 'regex':
        return bool(re.search(p['pattern'], answer, re.I)), "regex match"

    if t == 'length':
        words = len(answer.split())
        ok = p.get('min', 0) <= words <= p.get('max', 1_000_000)
        return ok, f"words={words}"

    if t == 'embedding':
        score = embedding_similarity(answer, p['target'])
        return score >= p['threshold'], f"sim={score:.3f}"

    if t == 'json_schema':
        try:
            obj = json.loads(answer)
        except Exception:
            return False, "invalid json"
        # TODO: validate against schema p['schema']
        return True, "json parsed"

    if t == 'code_test':
        # Placeholder: run in isolated sandbox
        return False, "code_test not implemented"

    if t == 'citation':
        # Placeholder: verify presence of citations
        return False, "citation check not implemented"

    return False, "unknown evaluation type"


# --- Evaluation of a single answer ---
def evaluate_answer(answer: str, evaluation_points: List[Dict[str, Any]]) -> Tuple[float, List[Dict]]:
    """
    Evaluate an answer against a list of atomic evaluation points.
    Returns a score (0–1) and a list of detailed results.
    """
    results = []
    for ep in evaluation_points:
        ok, note = run_evaluation_point(answer, ep)
        results.append({'point': ep['text'], 'ok': ok, 'note': note})
    score = sum(1 for r in results if r['ok']) / max(1, len(results))
    return score, results


# --- Comparison of multiple answers ---
def compare_answers(answers: List[str], evaluation_points_per_topic: List[List[Dict[str, Any]]]):
    """
    Compare multiple answers (from different models) for the same research topics.
    Selects the highest-scoring answer per topic.
    """
    scores = []
    for ans, points in zip(answers, evaluation_points_per_topic):
        s, details = evaluate_answer(ans, points)
        scores.append((s, details))

    winner_index = max(range(len(scores)), key=lambda i: scores[i][0])
    return winner_index, scores


# --- Example of usage ---
if __name__ == "__main__":
    # Example research topic and generated evaluation points
    research_topic = "Explain the concept of TissLang and give two code examples."

    evaluation_points = [
        {"text": "The answer clearly defines TissLang's purpose.",
         "type": "keyword",
         "params": {"keywords": ["TissLang"], "min_count": 1}},
        {"text": "The answer provides at least two distinct code examples.",
         "type": "regex",
         "params": {"pattern": r"```.*?```.*?```.*?```"}},
        {"text": "The answer is at least 100 words long.",
         "type": "length",
         "params": {"min": 100}}
    ]

    answers = [
        "TissLang is ... (model 1 output)...",
        "TissLang is ... (model 2 output)..."
    ]

    winner, score_details = compare_answers(answers, [evaluation_points] * len(answers))
    print(f"Best answer: Model {winner}")
    for idx, (score, details) in enumerate(score_details):
        print(f"Model {idx} score: {score}")
        for d in details:
            print(f"  - {d['point']}: {'PASS' if d['ok'] else 'FAIL'} ({d['note']})")
