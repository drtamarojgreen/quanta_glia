import os
import shutil
import pytest
import csv
import json
from pathlib import Path
from scripts.harvester import extract_text_from_repo
from scripts.reporting import analyze_repo_files, write_to_csv
from scripts.pruner import build_dependency_map

@pytest.fixture
def temp_repo(tmp_path):
    repo_path = tmp_path / "test_repo"
    repo_path.mkdir()
    (repo_path / "docs").mkdir()
    (repo_path / "README.md").write_text("Line 1\nLine 2\nLine 3\nLine 4\nLine 5")
    (repo_path / "docs" / "usage.md").write_text("How to use this repo.")
    (repo_path / "main.py").write_text("print('hello')")
    (repo_path / "requirements.txt").write_text("other_repo\n")
    return repo_path

def test_harvester_selective_extraction(temp_repo):
    # Test max_depth=0 (only root)
    extracted = extract_text_from_repo(temp_repo, ["README", "usage"], search_paths=["/"], max_depth=0)
    assert "README.md" in extracted
    assert "usage.md" not in extracted

    # Test max_depth=1 (includes docs)
    extracted = extract_text_from_repo(temp_repo, ["README", "usage"], search_paths=["/"], max_depth=1)
    assert "README.md" in extracted
    assert "usage.md" in extracted

    # Test max_lines_per_file
    extracted = extract_text_from_repo(temp_repo, ["README"], max_lines_per_file=2)
    assert len(extracted["README.md"].splitlines()) == 2

def test_reporting_tech_stack(temp_repo):
    num_files, num_src_files, num_doc_files, num_test_files, extension_counts = analyze_repo_files(temp_repo)
    assert extension_counts[".py"] == 1
    # .md is not in src_exts in the current implementation, so it won't be in extension_counts
    assert ".md" not in extension_counts

    tech_stack = max(extension_counts, key=extension_counts.get)
    assert tech_stack == ".py"

def test_reporting_csv_format(tmp_path):
    csv_file = tmp_path / "test_report.csv"
    data = {
        'timestamp': '2023-10-27T10:00:00',
        'repository_name': 'test_repo',
        'repository_size': 100,
        'number_files': 5,
        'number_src_files': 1,
        'number_doc_files': 2,
        'number_test_files': 0,
        'tech_stack': '.py',
    }
    write_to_csv(data, filename=str(csv_file))

    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        row = next(reader)
        assert 'timestamp' in row
        assert 'tech_stack' in row
        assert row['tech_stack'] == '.py'

def test_pruner_dependency_map(tmp_path):
    kb_path = tmp_path / "knowledge_base"
    kb_path.mkdir()

    repo1 = kb_path / "repo1"
    repo1.mkdir()
    (repo1 / "requirements.txt").write_text("repo2\n")

    repo2 = kb_path / "repo2"
    repo2.mkdir()

    dep_map = build_dependency_map(kb_path)
    assert "repo2" in dep_map
    assert "repo1" in dep_map["repo2"]
