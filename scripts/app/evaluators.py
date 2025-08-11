"""
This module contains the core logic for running a single, atomic check.
"""
import re
from typing import Any, Dict, Tuple

# Placeholder for heavier dependencies that might be loaded optionally
EMBEDDING_CHECK = None
JSON_SCHEMA_CHECK = None
CODE_TEST_CHECK = None

def run_evaluation_point(answer: str, point: Dict[str, Any]) -> Tuple[bool, str]:
    """
    Dispatches to the correct validator based on the evaluation point's type.
    """
    t = point.get("type")
    p = point.get("params", {})

    if t == "keyword":
        keywords = p.get("keywords", [])
        min_count = p.get("min_count", 1)
        found_count = sum(1 for k in keywords if k.lower() in answer.lower())
        ok = found_count >= min_count
        note = f"Found {found_count}/{min_count} required keywords."
        return ok, note

    elif t == "regex":
        pattern = p.get("pattern")
        if not pattern:
            return False, "Regex pattern is missing in params."
        ok = bool(re.search(pattern, answer, re.DOTALL))
        note = f"Pattern '{pattern}' {'found' if ok else 'not found'}."
        return ok, note

    elif t == "length":
        words = len(answer.split())
        min_len = p.get("min")
        max_len = p.get("max")
        if min_len is not None and words < min_len:
            return False, f"Answer is too short ({words} words, min {min_len})."
        if max_len is not None and words > max_len:
            return False, f"Answer is too long ({words} words, max {max_len})."
        return True, f"Length is OK ({words} words)."

    elif t == "citation":
        pattern = p.get("pattern", r'\[\d+\]')
        min_count = p.get("min_count", 1)
        found_count = len(re.findall(pattern, answer))
        ok = found_count >= min_count
        note = f"Found {found_count}/{min_count} citations."
        return ok, note

    # --- Placeholders for more complex validators ---
    elif t == "embedding":
        return False, "Embedding validator not yet implemented."
    elif t == "json_schema":
        return False, "JSON schema validator not yet implemented."
    elif t == "code_test":
        return False, "Code test validator not yet implemented."

    else:
        return False, f"Unknown validator type: '{t}'"