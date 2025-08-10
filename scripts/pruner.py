#!/usr/bin/env python3

"""
QuantaGlia-Pruner - Phase 1 Implementation

This script implements the foundational pruner. It identifies and archives
repositories based on a simple, configurable age threshold. It is designed
to be safe, with dry-run capabilities and structured logging.
"""

import os
import sys
import logging
import shutil
import argparse
import json
from datetime import datetime, timedelta
from pathlib import Path

# Add project root to Python path for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config, JsonFormatter

def setup_logging(verbose: bool):
    """Configures structured (JSONL) logging."""
    log_level = logging.DEBUG if verbose else logging.INFO
    log_file = 'quantaglia.log'

    # Get the root logger
    logger = logging.getLogger()
    logger.setLevel(log_level)

    # Remove any existing handlers to avoid duplicate logs
    if logger.hasHandlers():
        logger.handlers.clear()

    # Create a file handler and set the formatter
    file_handler = logging.FileHandler(log_file, mode='a')
    formatter = JsonFormatter()
    file_handler.setFormatter(formatter)

    # Add the handler to the root logger
    logger.addHandler(file_handler)

    # Add a stream handler to print to console as well, but only for INFO and above
    # unless in verbose mode. This provides user-facing feedback.
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(log_level)
    console_handler.setFormatter(logging.Formatter('%(message)s')) # Simple format for console
    logger.addHandler(console_handler)

    logging.info({"event": "logging_configured", "level": logging.getLevelName(log_level)})


def run_pruning(args):
    """Contains the core logic for the age-based pruner."""
    setup_logging(args.verbose)
    logging.info({"event": "pruner_start", "dry_run": args.dry_run, "force": args.force})

    config = load_config("config.yaml")
    if not config:
        logging.error({"event": "config_load_failed", "reason": "Could not load or parse config.yaml"})
        return

    pruning_config = config.get('pruning', {})
    main_config = config.get('main', {})

    age_threshold = timedelta(days=pruning_config.get('age_threshold_days', 90))
    knowledge_base_path = Path(main_config.get('knowledge_base', './knowledge_base'))
    archive_path = Path(pruning_config.get('archive_path', 'repo_archive/'))

    if not knowledge_base_path.is_dir():
        logging.error({
            "event": "knowledge_base_not_found",
            "path": str(knowledge_base_path)
        })
        return

    logging.info({
        "event": "scan_start",
        "path": str(knowledge_base_path),
        "age_threshold_days": age_threshold.days
    })

    now = datetime.now()
    archived_count = 0

    for repo_path in knowledge_base_path.iterdir():
        if repo_path.is_dir():
            try:
                mtime = repo_path.stat().st_mtime
                last_modified_date = datetime.fromtimestamp(mtime)
                repo_age = now - last_modified_date

                if repo_age > age_threshold:
                    decision = "ARCHIVE"
                    reason = f"Exceeded age threshold of {age_threshold.days} days (age: {repo_age.days} days)"

                    log_payload = {
                        "event": "prune_decision",
                        "repo_name": repo_path.name,
                        "decision": decision,
                        "reason": reason,
                        "age_days": repo_age.days,
                        "dry_run": args.dry_run,
                        "actor": "QuantaGlia-Pruner"
                    }

                    if args.dry_run:
                        logging.warning(log_payload)
                    else:
                        archive_dest = archive_path / repo_path.name
                        log_payload["destination"] = str(archive_dest)

                        try:
                            # Ensure archive directory exists
                            archive_path.mkdir(exist_ok=True)
                            shutil.move(str(repo_path), str(archive_dest))
                            logging.info(log_payload)
                            archived_count += 1
                        except Exception as e:
                            log_payload["error"] = str(e)
                            logging.error(log_payload)

            except FileNotFoundError:
                logging.warning({
                    "event": "scan_error",
                    "repo_name": repo_path.name,
                    "reason": "File not found during scan, likely removed mid-process."
                })

    logging.info({"event": "pruner_end", "archived_count": archived_count, "dry_run": args.dry_run})


def main():
    """Parses command-line arguments and kicks off the pruner."""
    parser = argparse.ArgumentParser(description="QuantaGlia Knowledge Base Pruner (Phase 1: Foundational).")
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Perform a dry run, showing what would be pruned without making any changes."
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Force a run, ignoring any scheduling logic (not implemented in Phase 1)."
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable more detailed (DEBUG level) logging output."
    )
    args = parser.parse_args()
    run_pruning(args)


if __name__ == "__main__":
    main()
