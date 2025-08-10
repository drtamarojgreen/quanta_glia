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
import json
import subprocess
from datetime import datetime, timedelta
from pathlib import Path

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config

# Setup logging
logging.basicConfig(
    filename='quantaglia.log',
    filemode='a',
    level=logging.INFO, # Default to INFO, verbose flag will set it to DEBUG
    format='%(asctime)s [%(levelname)s] %(message)s'
)

def calculate_repo_score(repo_path: Path, pruning_config: dict) -> float:
    """
    Calculates a score for a repository based on age, key file presence, and size.

    The score is a weighted average of three components:
    1.  Age Score: Normalized based on the `age_threshold_days`. A brand new repo gets 100,
        a repo at the age threshold gets 0.
    2.  Key Files Score: Based on the presence of important files like README.md, LICENSE, etc.
    3.  Size Score: Based on the number of files in the repository, capped at a reasonable limit.

    Args:
        repo_path: The path to the repository directory.
        pruning_config: The pruning configuration dictionary from config.yaml.

    Returns:
        A score between 0 and 100.
    """
    now = datetime.now()
    weights = pruning_config.get('weights', {'age': 0.5, 'key_files': 0.3, 'size': 0.2})
    age_threshold_days = pruning_config.get('age_threshold_days', 30)
    key_files_list = pruning_config.get('key_files', [])

    # 1. Calculate Age Score
    try:
        mtime = repo_path.stat().st_mtime
        last_modified_date = datetime.fromtimestamp(mtime)
        age = now - last_modified_date
        age_score = max(0, 100 * (1 - (age.days / age_threshold_days)))
    except FileNotFoundError:
        return 0 # Repo was likely deleted mid-scan

    # 2. Calculate Key Files Score
    found_key_files = 0
    if key_files_list:
        for key_file in key_files_list:
            if (repo_path / key_file).exists():
                found_key_files += 1
        key_files_score = 100 * (found_key_files / len(key_files_list))
    else:
        key_files_score = 0

    # 3. Calculate Size Score (based on file count)
    num_files = sum(1 for _ in repo_path.glob('**/*') if _.is_file())
    # Normalize size score, let's say 50 files is a "good" size for max score.
    size_score = min(100, (num_files / 50.0) * 100)

    # 4. Calculate final weighted score
    final_score = (
        weights.get('age', 0.5) * age_score +
        weights.get('key_files', 0.3) * key_files_score +
        weights.get('size', 0.2) * size_score
    )

    logging.debug(
        f"Scoring for {repo_path.name}: "
        f"Age={age.days}d (Score: {age_score:.1f}), "
        f"KeyFiles={found_key_files}/{len(key_files_list)} (Score: {key_files_score:.1f}), "
        f"Size={num_files} files (Score: {size_score:.1f}), "
        f"Final Score: {final_score:.1f}"
    )

    return final_score


def run_pruning(args):
    """Contains the core logic for pruning."""
    if args.verbose:
        # Set root logger level to DEBUG
        logging.getLogger().setLevel(logging.DEBUG)
        logging.info("Verbose logging enabled.")

    logging.info("Starting QuantaGlia Pruner.")

    config_data = load_config(args.config)
    if config_data is None:
        logging.error("Failed to load configuration. Exiting.")
        sys.exit(1)

    main_config = config_data.get('main', {})
    pruning_config = config_data.get('pruning', {})

    KNOWLEDGE_BASE = Path(main_config.get("knowledge_base", "./knowledge_base"))
    SCORE_THRESHOLD = pruning_config.get("score_threshold", 50)

    # Strategy can be overridden by command line argument, otherwise use config
    strategy = args.strategy if args.strategy else pruning_config.get("strategy", "conservative")

    logging.info(f"Knowledge base: {KNOWLEDGE_BASE}")
    logging.info(f"Score threshold: {SCORE_THRESHOLD}")
    logging.info(f"Pruning strategy: {strategy}")

    if args.dry_run:
        logging.info("Performing a DRY RUN. No files will be changed.")
        print("--- DRY RUN ---")

    if not KNOWLEDGE_BASE.is_dir():
        logging.error(f"Knowledge base directory not found at: {KNOWLEDGE_BASE}")
        print(f"Error: Knowledge base directory not found at: {KNOWLEDGE_BASE}", file=sys.stderr)
        sys.exit(1)

    logging.info(f"Scanning {KNOWLEDGE_BASE} for repositories to prune.")

    for repo_dir in KNOWLEDGE_BASE.iterdir():
        if repo_dir.is_dir():
            try:
                score = calculate_repo_score(repo_dir, pruning_config)
                logging.info(f"Repository '{repo_dir.name}' scored {score:.2f}")

                if score < SCORE_THRESHOLD:
                    logging.warning(f"Repo '{repo_dir.name}' with score {score:.2f} is below threshold {SCORE_THRESHOLD}.")

                    if strategy == 'aggressive':
                        action_verb = "delete"
                        destination_info = ""
                    else: # 'conservative'
                        action_verb = "archive"
                        # Use a more generic archive folder, perhaps in parent dir
                        archive_dir = KNOWLEDGE_BASE.parent / "archive"
                        archive_dir.mkdir(exist_ok=True)
                        destination = archive_dir / f"{repo_dir.name}-score{score:.0f}"
                        destination_info = f" to {destination}"

                    if args.dry_run:
                        print(f"[DRY RUN] Would {action_verb} {repo_dir.name}{destination_info}")
                        logging.info(f"[DRY RUN] Would {action_verb} {repo_dir.name}{destination_info}")
                    else:
                        try:
                            if strategy == 'aggressive':
                                # Ethical check before deletion
                                ethos_script_path = Path(__file__).parent / 'quanta_ethos.py'
                                action_description = f"delete repository {repo_dir.name}"

                                try:
                                    # Ensure we use the same Python interpreter for the subprocess
                                    ethos_result = subprocess.run(
                                        [sys.executable, str(ethos_script_path), action_description],
                                        capture_output=True,
                                        text=True,
                                        check=True,
                                        encoding='utf-8'
                                    )
                                    ethos_response = json.loads(ethos_result.stdout)
                                    decision = ethos_response.get("decision", "deny")
                                    reason = ethos_response.get("reason", "No reason provided.")
                                    logging.debug(f"Ethos check for '{action_description}': {decision.upper()} - {reason}")

                                except FileNotFoundError:
                                    logging.error(f"Ethos script not found at {ethos_script_path}")
                                    decision = "deny"
                                    reason = "Ethos script not found."
                                except subprocess.CalledProcessError as e:
                                    logging.error(f"Ethos script execution failed for {repo_dir.name}: {e.stderr}")
                                    decision = "deny"
                                    reason = f"Ethos script failed: {e.stderr}"
                                except json.JSONDecodeError as e:
                                    logging.error(f"Failed to parse Ethos response for {repo_dir.name}: {e}")
                                    decision = "deny"
                                    reason = "Could not parse Ethos response."


                                if decision == "approve":
                                    shutil.rmtree(repo_dir)
                                    print(f"Deleted {repo_dir.name}")
                                    logging.info(f"Deleted {repo_dir.name} after ethos approval.")
                                else:
                                    message = f"Deletion of {repo_dir.name} DENIED by QuantaEthos. Reason: {reason}. Flagged for human review."
                                    print(message)
                                    logging.warning(message)

                            else: # 'conservative'
                                shutil.move(str(repo_dir), str(destination))
                                print(f"Archived {repo_dir.name}{destination_info}")
                                logging.info(f"Archived {repo_dir.name}{destination_info}")
                        except Exception as e:
                            logging.error(f"Failed to {action_verb} {repo_dir.name}: {e}")

            except FileNotFoundError:
                logging.warning(f"Could not stat directory {repo_dir}, it may have been removed mid-scan.")


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
        "--strategy",
        choices=["conservative", "aggressive"],
        default=None,
        help="Override the pruning strategy from config.yaml (conservative=archive, aggressive=delete)."
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
