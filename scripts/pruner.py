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

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config, setup_pruner_logger

def run_pruning(args):
    """Contains the core logic for pruning."""
    logger = setup_pruner_logger()
    logger.info("Starting QuantaGlia Pruner.")

    config_data = load_config(args.config)
    if config_data is None:
        logger.error("Failed to load configuration. Exiting.")
        sys.exit(1)

    main_config = config_data.get('main', {})
    pruning_config = config_data.get('pruning', {})

    KNOWLEDGE_BASE = Path(main_config.get("knowledge_base", "./knowledge_base"))
    AGE_THRESHOLD_DAYS = pruning_config.get("age_threshold_days", 30)

    logger.info(f"Knowledge base: {KNOWLEDGE_BASE}")
    logger.info(f"Age threshold (days): {AGE_THRESHOLD_DAYS}")

    if args.dry_run:
        logger.info("Performing a DRY RUN. No files will be changed.")
        print("--- DRY RUN ---")

    if not KNOWLEDGE_BASE.is_dir():
        logger.error(f"Knowledge base directory not found at: {KNOWLEDGE_BASE}")
        print(f"Error: Knowledge base directory not found at: {KNOWLEDGE_BASE}", file=sys.stderr)
        sys.exit(1)

    now = datetime.now()
    age_threshold = timedelta(days=AGE_THRESHOLD_DAYS)

    logger.info(f"Scanning {KNOWLEDGE_BASE} for repositories older than {AGE_THRESHOLD_DAYS} days.")

    for repo_dir in KNOWLEDGE_BASE.iterdir():
        if repo_dir.is_dir():
            try:
                mtime = repo_dir.stat().st_mtime
                last_modified_date = datetime.fromtimestamp(mtime)
                age = now - last_modified_date

                if age > age_threshold:
                    log_extra = {
                        "event": "prune_decision",
                        "repo_name": repo_dir.name,
                        "decision": "ARCHIVE",
                        "reason": f"Repository age ({age.days} days) exceeds threshold ({AGE_THRESHOLD_DAYS} days).",
                        "age_days": age.days,
                        "dry_run": args.dry_run,
                    }
                    logger.info(f"Found old repository: {repo_dir.name}. Preparing to archive.", extra=log_extra)

                    archive_dir = Path("archive")
                    archive_dir.mkdir(exist_ok=True)
                    new_name = f"archive-{repo_dir.name}-age{age.days}"
                    destination = archive_dir / new_name

                    if args.dry_run:
                        print(f"[DRY RUN] Would archive {repo_dir.name} to {destination}")
                        logger.info(f"Dry run: Would archive {repo_dir.name}", extra=log_extra)
                    else:
                        try:
                            shutil.move(str(repo_dir), str(destination))
                            print(f"Archived {repo_dir.name} to {destination}")
                            # Update decision for the log record
                            log_extra["decision"] = "ARCHIVED"
                            logger.info(f"Archived {repo_dir.name} to {destination}", extra=log_extra)
                        except Exception as e:
                            logger.error(f"Failed to archive {repo_dir.name}: {e}")
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
    args = parser.parse_args()
    run_pruning(args)


if __name__ == "__main__":
    main()
