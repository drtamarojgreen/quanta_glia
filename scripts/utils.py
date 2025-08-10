import os
import subprocess
import shutil
import logging
from pathlib import Path
import sys
import logging
import json
from datetime import datetime

class JsonFormatter(logging.Formatter):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def format(self, record):
        log_record = {
            "timestamp": datetime.utcfromtimestamp(record.created).isoformat() + "Z",
            "level": record.levelname,
            "message": record.getMessage(),
            "name": record.name,
            "filename": record.filename,
            "funcName": record.funcName,
            "lineno": record.lineno,
        }
        if hasattr(record, "correlation_id"):
            log_record["correlation_id"] = record.correlation_id
        return json.dumps(log_record)


def setup_logger(log_path='quantaglia.log', level=logging.INFO, name='QuantaGlia'):
    """
    Sets up a generic logger that uses the JsonFormatter.
    """
    logger = logging.getLogger(name)
    logger.setLevel(level)
    logger.propagate = False

    # Prevent duplicate handlers
    if logger.hasHandlers():
        logger.handlers.clear()

    # File handler with JSON formatter
    file_handler = logging.FileHandler(log_path, mode='a')
    file_handler.setFormatter(JsonFormatter())
    logger.addHandler(file_handler)

    # Console handler for errors
    console_handler = logging.StreamHandler(sys.stderr)
    console_handler.setLevel(logging.ERROR)
    console_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
    logger.addHandler(console_handler)

    return logger

def load_config(config_path="config.yaml"):
    """
    Loads configuration from a given YAML file path.
    A simple YAML parser for a specific key-value and list structure.
    Does not support nested maps or complex structures.
    """
    config = {}
    try:
        with open(config_path, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        logging.error(f"Configuration file not found at {config_path}")
        return None

    current_section = None
    current_list_key = None

    for raw_line in lines:
        indentation = len(raw_line) - len(raw_line.lstrip(' '))
        line = raw_line.strip()

        if not line or line.startswith('#'):
            continue

        if indentation == 0:
            current_section = line.replace(':', '').strip()
            config[current_section] = {}
            current_list_key = None
            continue

        if current_section and indentation > 0:
            if line.strip().startswith('-'):
                if current_list_key:
                    value = line.replace('-', '').strip().strip("'\"")
                    config[current_section][current_list_key].append(value)
            else:
                try:
                    key, value = line.split(':', 1)
                    key = key.strip()
                    value = value.strip()
                except ValueError:
                    logging.warning(f"Could not parse line: {line}")
                    continue

                if not value:
                    current_list_key = key
                    config[current_section][current_list_key] = []
                else:
                    current_list_key = None
                    if value.isdigit():
                        value = int(value)
                    else:
                        value = value.strip("'\"")
                    config[current_section][key] = value
    return config

def clone_repo(repo_url, repo_cache_path):
    """
    Clones a repository from a URL or copies it from a local path into the repo cache.
    """
    repo_name = repo_url.split('/')[-1].replace('.git', '')
    dest_path = repo_cache_path / repo_name
    if dest_path.exists():
        logging.info(f"Repo {repo_name} already in cache. Skipping.")
        return dest_path

    # Check if the repo_url is a local directory
    if os.path.isdir(repo_url):
        logging.info(f"Local directory detected at {repo_url}. Copying to cache.")
        try:
            shutil.copytree(repo_url, dest_path)
            logging.info(f"Copied local directory: {repo_url} to {dest_path}")
            return dest_path
        except Exception as e:
            logging.error(f"Failed to copy local directory {repo_url}: {e}")
            return None

    # If not a local directory, assume it's a git repository URL
    try:
        subprocess.run(["git", "clone", "--depth=1", repo_url, str(dest_path)], check=True)
        logging.info(f"Cloned repo: {repo_url}")
        return dest_path
    except subprocess.CalledProcessError:
        logging.error(f"Failed to clone repo: {repo_url}")
        return None

def prune_cache(repo_cache_path):
    """
    Removes all directories from the repo cache.
    """
    # Ensure the cache directory exists before trying to iterate over it
    if not repo_cache_path.exists():
        logging.warning(f"Repo cache directory not found at {repo_cache_path}. Nothing to prune.")
        return

    for repo_dir in repo_cache_path.iterdir():
        if repo_dir.is_dir():
            shutil.rmtree(repo_dir)
    logging.info(f"Pruned repo cache at {repo_cache_path}.")

# --- New Logging Implementation from Phase I ---

class PrunerJsonFormatter(logging.Formatter):
    """
    A custom logging formatter that outputs log records in a structured JSONL
    format, as specified for the QuantaGlia-Pruner.
    """
    def format(self, record):
        """
        Formats a log record into a JSON string.

        The log record is expected to have additional data passed via the
        `extra` dictionary when logging.

        Required extra fields:
        - event (str): The type of event being logged (e.g., 'prune_decision').
        - repo_name (str): The name of the repository being evaluated.
        - decision (str): The action taken (e.g., 'ARCHIVE', 'DELETE', 'KEEP').
        - reason (str): The justification for the decision.
        - age_days (int): The calculated age of the repository in days.
        - dry_run (bool): Whether the operation was a dry run.
        """
        # Start with the basic structure
        log_object = {
            "timestamp": datetime.utcfromtimestamp(record.created).isoformat() + "Z",
            "actor": "QuantaGlia-Pruner",
        }

        # Add fields from the `extra` dictionary, providing defaults
        log_object.update({
            "event": getattr(record, "event", "undefined"),
            "repo_name": getattr(record, "repo_name", "n/a"),
            "decision": getattr(record, "decision", "n/a"),
            "reason": getattr(record, "reason", "n/a"),
            "age_days": getattr(record, "age_days", -1),
            "dry_run": getattr(record, "dry_run", False),
        })

        # Add the standard message if it exists and is not the main dict
        if record.getMessage():
             log_object['message'] = record.getMessage()

        return json.dumps(log_object)

def setup_pruner_logger(log_path='quantaglia.log', level=logging.INFO):
    """
    Sets up a dedicated logger for the pruner that uses the PrunerJsonFormatter.

    This creates a new logger instance to avoid interfering with the root
    logger or other loggers that may be in use.

    Args:
        log_path (str): The path to the log file.
        level (int): The logging level (e.g., logging.INFO).

    Returns:
        logging.Logger: A configured logger instance.
    """
    logger = logging.getLogger("QuantaGlia-Pruner")
    logger.setLevel(level)
    logger.propagate = False  # Prevent logs from bubbling up to the root logger

    # Remove any existing handlers to prevent duplicate output
    if logger.hasHandlers():
        logger.handlers.clear()

    # Create a file handler to write to the log file
    handler = logging.FileHandler(log_path, mode='a')
    handler.setFormatter(PrunerJsonFormatter())
    logger.addHandler(handler)

    return logger
