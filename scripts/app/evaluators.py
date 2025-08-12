"""
This module contains the core logic for running a single, atomic check.
"""
import re
from typing import Any, Dict, Tuple, List

# Placeholder for heavier dependencies that might be loaded optionally
EMBEDDING_CHECK = None
JSON_SCHEMA_CHECK = None
CODE_TEST_CHECK = None

def run_evaluation_point(answer: str, point: Dict[str, Any]) -> Tuple[bool, str, Any]:
    """
    Dispatches to the correct validator based on the evaluation point's type.

    Returns:
        A tuple of (ok, note, evidence).
    """
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
        return ok, note, evidence

    elif t == "regex":
        pattern = p.get("pattern")
        if not pattern:
            return False, "Regex pattern is missing in params.", None
        match = re.search(pattern, answer, re.DOTALL)
        ok = bool(match)
        note = f"Pattern '{pattern}' {'found' if ok else 'not found'}."
        evidence = match.group(0) if match else None
        return ok, note, evidence

    elif t == "length":
        words = len(answer.split())
        min_len = p.get("min")
        max_len = p.get("max")
        if min_len is not None and words < min_len:
            return False, f"Answer is too short ({words} words, min {min_len}).", words
        if max_len is not None and words > max_len:
            return False, f"Answer is too long ({words} words, max {max_len}).", words
        return True, f"Length is OK ({words} words).", words

    elif t == "citation":
        pattern = p.get("pattern", r'\[\d+\]')
        min_count = p.get("min_count", 1)
        found_citations = re.findall(pattern, answer)
        found_count = len(found_citations)
        ok = found_count >= min_count
        note = f"Found {found_count}/{min_count} citations."
        evidence = found_citations if found_citations else None
        return ok, note, evidence

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
        return ok, note, evidence

    # --- Placeholders for more complex validators ---
    elif t == "embedding":
        return False, "Embedding validator not yet implemented.", None
    elif t == "json_schema":
        return False, "JSON schema validator not yet implemented.", None
    elif t == "code_test":
        return False, "Code test validator not yet implemented.", None

    else:
        return False, f"Unknown validator type: '{t}'", None