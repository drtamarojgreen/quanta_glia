import unittest
from unittest.mock import MagicMock
from datetime import datetime, timedelta
from pathlib import Path
import sys

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from scripts.pruner import (
    RepoRecord,
    calculate_composite_score,
    make_pruning_decision,
    get_ethical_verdict,
)

class TestPrunerLogic(unittest.TestCase):
    """
    Unit tests for the core decision-making logic of the pruner,
    as specified in the Phase II implementation plan.
    """

    def setUp(self):
        """Set up common test data."""
        self.mock_weights = {
            'usage': 0.5,
            'age': 0.2,
            'redundancy': 0.2,
            'ethics_risk': 0.1
        }
        self.mock_thresholds = {
            'delete': 0.9,
            'archive': 0.7,
            'review': 0.5
        }
        self.base_repo = RepoRecord(
            name="test-repo",
            path=Path("/fake/path"),
            last_activity_at=datetime.now()
        )

    def test_calculate_composite_score_low_score(self):
        """Test a young, used repo results in a low score."""
        self.base_repo.usage_score = 1.0 # High usage
        age_days = 10
        score = calculate_composite_score(self.base_repo, age_days, self.mock_weights)
        # Only age contributes a small amount
        self.assertLess(score, 0.1)

    def test_calculate_composite_score_high_score(self):
        """Test an old, unused, redundant repo results in a high score."""
        self.base_repo.usage_score = 0.0 # No usage
        self.base_repo.redundancy_score = 1.0 # Highly redundant
        age_days = 365 # Max age for normalization
        score = calculate_composite_score(self.base_repo, age_days, self.mock_weights)
        # usage(0.5) + age(0.2) + redundancy(0.2) = 0.9
        self.assertAlmostEqual(score, 0.9)

    def test_calculate_composite_score_ethics_risk(self):
        """Test that ethics risk contributes to the score."""
        self.base_repo.ethics_risk_score = 1.0
        age_days = 0
        score = calculate_composite_score(self.base_repo, age_days, self.mock_weights)
        # Only ethics risk contributes
        self.assertAlmostEqual(score, 0.1)

    def test_make_pruning_decision(self):
        """Test decision making based on score thresholds."""
        # Test DELETE threshold
        decision, reason = make_pruning_decision(0.95, self.mock_thresholds)
        self.assertEqual(decision, "DELETE")
        self.assertIn("DELETE threshold", reason)

        # Test ARCHIVE threshold
        decision, reason = make_pruning_decision(0.8, self.mock_thresholds)
        self.assertEqual(decision, "ARCHIVE")
        self.assertIn("ARCHIVE threshold", reason)

        # Test REVIEW threshold
        decision, reason = make_pruning_decision(0.6, self.mock_thresholds)
        self.assertEqual(decision, "REVIEW")
        self.assertIn("REVIEW threshold", reason)

        # Test KEEP threshold
        decision, reason = make_pruning_decision(0.4, self.mock_thresholds)
        self.assertEqual(decision, "KEEP")
        self.assertIn("below all action thresholds", reason)

    def test_get_ethical_verdict_allow(self):
        """Test that a normal repo is allowed."""
        verdict, reason = get_ethical_verdict("normal-repo", "DELETE")
        self.assertEqual(verdict, "ALLOW")

    def test_get_ethical_verdict_deny_critical(self):
        """Test that a 'critical' repo is denied."""
        verdict, reason = get_ethical_verdict("critical-project", "DELETE")
        self.assertEqual(verdict, "DENY")
        self.assertIn("protected repository", reason)

    def test_get_ethical_verdict_deny_protected(self):
        """Test that a 'protected' repo is denied."""
        verdict, reason = get_ethical_verdict("some-protected-data", "ARCHIVE")
        self.assertEqual(verdict, "DENY")

    def test_get_ethical_verdict_non_destructive_action(self):
        """Test that non-destructive actions are always allowed."""
        verdict, reason = get_ethical_verdict("critical-project", "KEEP")
        self.assertEqual(verdict, "ALLOW")

if __name__ == '__main__':
    unittest.main()