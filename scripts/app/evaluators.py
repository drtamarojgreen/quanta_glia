"""
This module contains the core logic for running a single, atomic check.
"""
import re
import time
import difflib
from typing import Any, Dict, Tuple, List

# Placeholder for heavier dependencies that might be loaded optionally
EMBEDDING_CHECK = None
JSON_SCHEMA_CHECK = None
CODE_TEST_CHECK = None

def run_evaluation_point(answer: str, point: Dict[str, Any]) -> Tuple[bool, str, Any, float]:
    """
    Dispatches to the correct validator based on the evaluation point's type.

    Returns:
        A tuple of (ok, note, evidence, duration_ms).
    """
    start_time = time.perf_counter()
    try:
        t = point.get("type")
        p = point.get("params", {})

        if t == "keyword":
            keywords = p.get("keywords", [])
            min_count = p.get("min_count", 1)
            found_keywords = [k for k in keywords if k.lower() in answer.lower()]
            found_count = len(found_keywords)
            ok = found_count >= min_count
            note = f"Found {found_count}/{min_count} required keywords."
            evidence = f"Found: {found_keywords}" if found_keywords else None
            result = (ok, note, evidence)

        elif t == "regex":
            pattern = p.get("pattern")
            if not pattern:
                result = (False, "Regex pattern is missing in params.", None)
            else:
                match = re.search(pattern, answer, re.DOTALL)
                ok = bool(match)
                note = f"Pattern '{pattern}' {'found' if ok else 'not found'}."
                evidence = match.group(0) if match else None
                result = (ok, note, evidence)

        elif t == "length":
            words = len(answer.split())
            min_len = p.get("min")
            max_len = p.get("max")
            if min_len is not None and words < min_len:
                result = (False, f"Answer is too short ({words} words, min {min_len}).", words)
            elif max_len is not None and words > max_len:
                result = (False, f"Answer is too long ({words} words, max {max_len}).", words)
            else:
                result = (True, f"Length is OK ({words} words).", words)

        elif t == "citation":
            pattern = p.get("pattern", r'\[\d+\]')
            min_count = p.get("min_count", 1)
            found_citations = re.findall(pattern, answer)
            found_count = len(found_citations)
            ok = found_count >= min_count
            note = f"Found {found_count}/{min_count} citations."
            evidence = found_citations if found_citations else None
            result = (ok, note, evidence)

        elif t == "negation":
            forbidden_keywords = p.get("keywords", [])
            found_keywords = [k for k in forbidden_keywords if k.lower() in answer.lower()]
            ok = not found_keywords  # Pass if the list is empty
            if ok:
                note = "No forbidden keywords found."
                evidence = None
            else:
                note = f"Found forbidden keywords: {found_keywords}"
                evidence = found_keywords
            result = (ok, note, evidence)

        elif t == "diff":
            golden_answer = p.get("golden_answer")
            if not golden_answer:
                result = (False, "Golden answer is missing in params.", None)
            else:
                threshold = p.get("threshold", 0.9)
                matcher = difflib.SequenceMatcher(None, answer, golden_answer)
                ratio = matcher.ratio()

                ok = ratio >= threshold

                if ok:
                    note = f"Similarity ratio {ratio:.2f} meets threshold {threshold:.2f}."
                    evidence = f"Ratio: {ratio:.2f}"
                else:
                    note = f"Similarity ratio {ratio:.2f} is below threshold {threshold:.2f}."
                    diff_lines = difflib.unified_diff(
                        golden_answer.splitlines(keepends=True),
                        answer.splitlines(keepends=True),
                        fromfile='golden_answer',
                        tofile='llm_answer',
                    )
                    evidence = ''.join(diff_lines) if any(diff_lines) else f"Ratio: {ratio:.2f}"
                result = (ok, note, evidence)

        # --- Placeholders for more complex validators ---
        elif t == "embedding":
            result = (False, "Embedding validator not yet implemented.", None)
        elif t == "json_schema":
            result = (False, "JSON schema validator not yet implemented.", None)
        elif t == "code_test":
            result = (False, "Code test validator not yet implemented.", None)

        else:
            result = (False, f"Unknown validator type: '{t}'", None)

    finally:
        duration_ms = (time.perf_counter() - start_time) * 1000

    return *result, duration_ms