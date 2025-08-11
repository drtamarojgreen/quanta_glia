"""
Unit tests for the research integration and evaluation framework.
"""
import pytest
from scripts.app import evaluators, scoring, research_utils

# --- Fixtures ---

@pytest.fixture
def user_schema():
    """A reusable JSON schema for tests."""
    return {
        "type": "object",
        "properties": {
            "user_id": {"type": "number"},
            "user_name": {"type": "string"},
        },
        "required": ["user_id", "user_name"]
    }

# --- Tests for evaluators.py ---

@pytest.mark.parametrize("answer, params, expected, note_part", [
    ("The quick brown fox.", {"keywords": ["fox"], "min_count": 1}, True, "found 1/1"),
    ("The quick brown fox.", {"keywords": ["dog"], "min_count": 1}, False, "found 0/1"),
    ("Case matters, case.", {"keywords": ["case"], "min_count": 2}, True, "found 2/2"),
    ("Case matters, CASE.", {"keywords": ["case"], "min_count": 2}, True, "found 2/2"),
    ("Just one keyword.", {"keywords": ["keyword", "missing"], "min_count": 2}, False, "found 1/2"),
])
def test_keyword_validator(answer, params, expected, note_part):
    point = {"type": "keyword", "params": params}
    ok, note = evaluators.run_evaluation_point(answer, point)
    assert ok is expected
    assert note_part in note

@pytest.mark.parametrize("answer, pattern, expected", [
    ("Function: my_func()", r"my_func\(\)", True),
    ("No function here.", r"my_func\(\)", False),
    ("Multi\nline\nmatch", r"Multi.*match", True),
])
def test_regex_validator(answer, pattern, expected):
    point = {"type": "regex", "params": {"pattern": pattern}}
    ok, _ = evaluators.run_evaluation_point(answer, point)
    assert ok is expected

@pytest.mark.parametrize("answer, params, expected", [
    ("One two three", {"min": 3, "max": 3}, True),
    ("One two", {"min": 3}, False),
    ("One two three four", {"max": 3}, False),
    ("One two three", {"min": 1}, True),
])
def test_length_validator(answer, params, expected):
    point = {"type": "length", "params": params}
    ok, _ = evaluators.run_evaluation_point(answer, point)
    assert ok is expected

def test_embedding_validator(monkeypatch):
    # Mock the similarity function to isolate the validator logic
    monkeypatch.setattr(research_utils, "embedding_similarity", lambda a, b: 0.85)
    monkeypatch.setattr(research_utils, "_SENTENCE_TRANSFORMERS_AVAILABLE", True)

    point = {"type": "embedding", "params": {"target": "any", "threshold": 0.8}}
    ok, note = evaluators.run_evaluation_point("some text", point)
    assert ok is True
    assert "sim=0.850" in note

    point_fail = {"type": "embedding", "params": {"target": "any", "threshold": 0.9}}
    ok, _ = evaluators.run_evaluation_point("some text", point_fail)
    assert ok is False

def test_embedding_validator_not_installed(monkeypatch):
    monkeypatch.setattr(research_utils, "_SENTENCE_TRANSFORMERS_AVAILABLE", False)
    point = {"type": "embedding", "params": {"target": "any", "threshold": 0.8}}
    ok, note = evaluators.run_evaluation_point("some text", point)
    assert ok is False
    assert note == "sentence-transformers not installed"

def test_json_schema_validator(monkeypatch, user_schema):
    monkeypatch.setattr(research_utils, "jsonschema", __import__("jsonschema"))

    # Valid case
    point = {"type": "json_schema", "params": {"schema": user_schema}}
    answer = '{"user_id": 123, "user_name": "test"}'
    ok, note = evaluators.run_evaluation_point(answer, point)
    assert ok is True
    assert note == "json schema valid"

    # Invalid JSON
    answer_invalid_json = "{'user_id': 123}"
    ok, note = evaluators.run_evaluation_point(answer_invalid_json, point)
    assert ok is False
    assert "invalid json" in note

    # Fails schema validation
    answer_bad_schema = '{"user_id": 123}'
    ok, note = evaluators.run_evaluation_point(answer_bad_schema, point)
    assert ok is False
    assert "schema validation failed" in note

def test_json_schema_validator_not_installed(monkeypatch):
    monkeypatch.setattr(research_utils, "jsonschema", None)
    point = {"type": "json_schema", "params": {"schema": {}}}
    ok, note = evaluators.run_evaluation_point("{}", point)
    assert ok is False
    assert note == "jsonschema not installed"

@pytest.mark.parametrize("answer, params, expected, note_part", [
    ("See ref [1] and [2].", {"min_count": 2}, True, "found 2/2"),
    ("See ref [1].", {"min_count": 2}, False, "found 1/2"),
    ("No refs.", {"min_count": 1}, False, "found 0/1"),
    ("Custom ref (Smith, 2021)", {"pattern": r'\(.+?, \d{4}\)', "min_count": 1}, True, "found 1/1"),
])
def test_citation_validator(answer, params, expected, note_part):
    point = {"type": "citation", "params": params}
    ok, note = evaluators.run_evaluation_point(answer, point)
    assert ok is expected
    assert note_part in note

def test_unknown_validator():
    point = {"type": "non_existent_type", "params": {}}
    ok, note = evaluators.run_evaluation_point("any answer", point)
    assert ok is False
    assert note == "unknown evaluation type: non_existent_type"


# --- Tests for scoring.py ---

def test_evaluate_answer():
    points = [
        {"text": "p1", "type": "keyword", "params": {"keywords": ["pass"]}},
        {"text": "p2", "type": "length", "params": {"min": 10}}, # This will fail
    ]
    answer = "This should pass"
    score, details = scoring.evaluate_answer(answer, points)
    assert score == 0.5
    assert len(details) == 2
    assert details[0]['ok'] is True
    assert details[1]['ok'] is False

def test_evaluate_answer_no_points():
    score, details = scoring.evaluate_answer("any answer", [])
    assert score == 1.0
    assert len(details) == 0

def test_compare_answers():
    points = [{"text": "p1", "type": "keyword", "params": {"keywords": ["good"]}}]
    answers = [
        "This is a bad answer", # Score 0.0
        "This is a good answer", # Score 1.0
        "This is also not good", # Score 0.0
    ]
    points_per_topic = [points] * len(answers)

    winner_index, score_details = scoring.compare_answers(answers, points_per_topic)
    assert winner_index == 1
    assert len(score_details) == 3
    assert score_details[0][0] == 0.0
    assert score_details[1][0] == 1.0
    assert score_details[2][0] == 0.0