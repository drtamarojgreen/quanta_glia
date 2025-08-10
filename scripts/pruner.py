#!/usr/bin/env python3

"""
QuantaGlia-Pruner

This script is responsible for the intelligent, automated maintenance of the
QuantaGlia knowledge base. It periodically evaluates repositories for
redundancy, obsolescence, and low impact, and then merges, archiving, or
deleting them based on configurable thresholds.
"""

import os
import sys
import logging
import shutil
import argparse
from datetime import datetime, timedelta
from pathlib import Path
from dataclasses import dataclass
from typing import List, Literal

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config, setup_pruner_logger

# --- Typed Data Models (as per Phase II spec) ---

DecisionType = Literal["DELETE", "ARCHIVE", "REVIEW", "KEEP"]

@dataclass
class RepoRecord:
    """A data class to hold information about a repository being evaluated."""
    name: str
    path: Path
    last_activity_at: datetime
    # Mocked scores for Phase II - real values will come from other Quanta modules
    usage_score: float = 0.0       # 0.0 (unused) to 1.0 (heavily used)
    redundancy_score: float = 0.0  # 0.0 (unique) to 1.0 (highly redundant)
    ethics_risk_score: float = 0.0 # 0.0 (no risk) to 1.0 (high risk)

@dataclass
class PruneDecision:
    """A data class to hold the outcome of a pruning evaluation."""
    repo_name: str
    decision: DecisionType
    final_score: float
    reasons: List[str]
    dry_run: bool
    policy_verdict: Literal["ALLOW", "DENY"] = "ALLOW"


# --- Core Logic Functions (as per Phase II spec) ---

def calculate_composite_score(repo: 'RepoRecord', age_days: int, weights: dict) -> float:
    """
    Calculates a composite score for a repository based on multiple factors.
    In Phase II, most scores are mocked. The age score is normalized.
    A higher score indicates a higher likelihood of being pruned.
    """
    # Normalize age. Let's cap normalization at 365 days for stability.
    # A higher age gives a higher score.
    max_age_for_norm = 365.0
    normalized_age = min(age_days / max_age_for_norm, 1.0)

    # Scores are assumed to be between 0.0 and 1.0
    # For usage, a lower score is worse, so we use (1 - usage_score)
    usage_contribution = (1.0 - repo.usage_score) * weights.get('usage', 0.0)
    age_contribution = normalized_age * weights.get('age', 0.0)
    redundancy_contribution = repo.redundancy_score * weights.get('redundancy', 0.0)
    ethics_contribution = repo.ethics_risk_score * weights.get('ethics_risk', 0.0)

    total_score = (
        usage_contribution +
        age_contribution +
        redundancy_contribution +
        ethics_contribution
    )
    return round(total_score, 4)

def make_pruning_decision(score: float, thresholds: dict) -> (DecisionType, str):
    """Makes a pruning decision based on the composite score and configured thresholds."""
    # Check from highest threshold to lowest
    if score >= thresholds.get('delete', 1.1): # Default to unachievable
        return "DELETE", f"Score ({score}) meets or exceeds DELETE threshold ({thresholds.get('delete')})"
    elif score >= thresholds.get('archive', 1.1):
        return "ARCHIVE", f"Score ({score}) meets or exceeds ARCHIVE threshold ({thresholds.get('archive')})"
    elif score >= thresholds.get('review', 1.1):
        return "REVIEW", f"Score ({score}) meets or exceeds REVIEW threshold ({thresholds.get('review')})"
    else:
        return "KEEP", f"Score ({score}) is below all action thresholds"

def get_ethical_verdict(repo_name: str, decision: DecisionType) -> (Literal["ALLOW", "DENY"], str):
    """
    Simulates a call to QuantaEthos for an ethical verdict on a proposed action.
    In Phase II, this is a simple mock. It will deny actions on repos with
    'critical' or 'protected' in their name.
    """
    if decision in ["DELETE", "ARCHIVE"]:
        # This check simulates a policy from QuantaEthos
        if "critical" in repo_name or "protected" in repo_name:
            return "DENY", "Ethical governor blocked action on a protected repository."
    return "ALLOW", "Action approved by ethical governor."

def run_pruning(args):
    """Contains the core logic for pruning."""
    logger = setup_pruner_logger()
    if args.verbose:
        logger.setLevel(logging.DEBUG)
        logger.debug("Verbose logging enabled by --verbose flag.")

    logger.info("Starting QuantaGlia Pruner.")

    config_data = load_config(args.config)
    if config_data is None:
        logger.error("Failed to load configuration. Exiting.")
        sys.exit(1)

    # --- Load Configuration ---
    main_config = config_data.get('main', {})
    pruning_config = config_data.get('pruning', {})

    KNOWLEDGE_BASE = Path(main_config.get("knowledge_base", "./knowledge_base"))
    ARCHIVE_PATH = Path(pruning_config.get("archive_path", "repo_archive/"))
    
    # Determine strategy: CLI flag > config file > default
    strategy = args.strategy or pruning_config.get("default_strategy", "conservative")
    
    weights = pruning_config.get('scoring_weights', {})
    thresholds = pruning_config.get('decision_thresholds', {})

    logger.info(f"Knowledge base: {KNOWLEDGE_BASE}")
    logger.info(f"Archive path: {ARCHIVE_PATH}")
    logger.info(f"Pruning strategy: {strategy.upper()}")

    if args.dry_run:
        logger.info("Performing a DRY RUN. No files will be changed.")
        print("--- DRY RUN ---")

    if not KNOWLEDGE_BASE.is_dir():
        logger.error(f"Knowledge base directory not found at: {KNOWLEDGE_BASE}")
        print(f"Error: Knowledge base directory not found at: {KNOWLEDGE_BASE}", file=sys.stderr)
        sys.exit(1)

    now = datetime.now()
    logger.info(f"Scanning {KNOWLEDGE_BASE} for repositories...")

    for repo_dir in KNOWLEDGE_BASE.iterdir():
        if repo_dir.is_dir():
            try:
                mtime = repo_dir.stat().st_mtime
                last_modified_date = datetime.fromtimestamp(mtime)
                age = now - last_modified_date
                
                repo_record = RepoRecord(
                    name=repo_dir.name,
                    path=repo_dir,
                    last_activity_at=last_modified_date
                )

                score = calculate_composite_score(repo_record, age.days, weights)
                decision, reason = make_pruning_decision(score, thresholds)

                prune_decision = PruneDecision(
                    repo_name=repo_record.name,
                    decision=decision,
                    final_score=score,
                    reasons=[reason],
                    dry_run=args.dry_run
                )

                # --- Ethical Governor Check ---
                policy_verdict, policy_reason = "ALLOW", "Action approved by ethical governor."
                if decision in ["DELETE", "ARCHIVE"]:
                    policy_verdict, policy_reason = get_ethical_verdict(repo_record.name, decision)
                
                prune_decision.policy_verdict = policy_verdict

                log_extra = {
                    "event": "prune_decision",
                    "repo_name": prune_decision.repo_name,
                    "decision": prune_decision.decision,
                    "reason": ", ".join(prune_decision.reasons),
                    "score": prune_decision.final_score,
                    "dry_run": prune_decision.dry_run,
                    "policy_verdict": prune_decision.policy_verdict,
                }

                if policy_verdict == "DENY":
                    logger.warning(f"Action '{decision}' on {repo_record.name} was blocked by ethical governor. Reason: {policy_reason}", extra=log_extra)
                    print(f"BLOCKED {decision} on {repo_record.name} by ethical governor.")
                    continue # Skip to the next repo

                logger.info(f"Evaluated {repo_record.name}: Score={score}, Decision={decision}", extra=log_extra)

                # --- Execute Action ---
                if decision == "ARCHIVE":
                    ARCHIVE_PATH.mkdir(exist_ok=True)
                    destination = ARCHIVE_PATH / repo_record.name
                    if args.dry_run:
                        print(f"[DRY RUN] Would archive {repo_record.name} to {destination}")
                    else:
                        try:
                            shutil.move(str(repo_record.path), str(destination))
                            print(f"Archived {repo_dir.name} to {destination}")
                            logger.info(f"Archived {repo_record.name}", extra=log_extra)
                        except Exception as e:
                            logger.error(f"Failed to archive {repo_record.name}: {e}", extra=log_extra)

                elif decision == "DELETE":
                    if strategy == "aggressive":
                        if args.dry_run:
                            print(f"[DRY RUN] Would DELETE {repo_record.name}")
                        else:
                            try:
                                shutil.rmtree(repo_record.path)
                                print(f"DELETED {repo_record.name}")
                                logger.info(f"Deleted {repo_record.name}", extra=log_extra)
                            except Exception as e:
                                logger.error(f"Failed to delete {repo_record.name}: {e}", extra=log_extra)
                    else:
                        logger.warning(f"DELETE action for {repo_record.name} blocked. Strategy is '{strategy}', not 'aggressive'.", extra=log_extra)
                        print(f"SKIPPED DELETE of {repo_record.name} (strategy is '{strategy}')")

                elif decision == "REVIEW":
                    print(f"Flagged for REVIEW: {repo_record.name} (Score: {score})")

            except FileNotFoundError:
                logger.warning(f"Could not stat directory {repo_dir}, it may have been removed.")


def main():
    """Parses command-line arguments and kicks off the pruner."""
    parser = argparse.ArgumentParser(description="QuantaGlia Knowledge Base Pruner.")
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Perform a dry run, showing what would be pruned without making changes."
    )
    parser.add_argument(
        "-c", "--config",
        default="config.yaml",
        help="Path to the configuration file (default: config.yaml)"
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Force a run, ignoring any configured schedule (feature not yet implemented)."
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Increase logging verbosity to DEBUG for this run."
    )
    parser.add_argument(
        "--strategy",
        choices=['conservative', 'aggressive'],
        default=None, # Default is read from config
        help="Override the pruning strategy for this run ('conservative' or 'aggressive')."
    )
    args = parser.parse_args()
    run_pruning(args)


if __name__ == "__main__":
    main()
