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
    A simple YAML parser that handles up to two levels of nesting.
    """
    config = {}
    try:
        with open(config_path, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        logging.error(f"Configuration file not found at {config_path}")
        return None

    stack = [(0, config)]

    for raw_line in lines:
        indentation = len(raw_line) - len(raw_line.lstrip(' '))
        line = raw_line.strip()

        if not line or line.startswith('#'):
            continue

        # Pop from stack if indentation decreases
        while len(stack) > 1 and indentation <= stack[-1][0]:
            stack.pop()

        current_dict = stack[-1][1]

        if line.startswith('-'):
            # It's a list item
            if isinstance(current_dict, list):
                value = line[1:].strip().strip("'\"")
                current_dict.append(value)
            continue

        if ':' in line:
            key, value = line.split(':', 1)
            key = key.strip()
            value = value.strip().strip("'\"")

            if not value:
                # Potential start of a nested dict or list
                # We don't know yet if it's a list or dict, assume dict for now
                # But if the NEXT line starts with '-', it's a list
                new_dict = {}
                current_dict[key] = new_dict
                stack.append((indentation, new_dict))
                # Peek ahead to see if it should be a list
            else:
                if value.isdigit():
                    value = int(value)
                elif value.lower() == 'true':
                    value = True
                elif value.lower() == 'false':
                    value = False
                elif '.' in value:
                    try:
                        value = float(value)
                    except ValueError:
                        pass
                current_dict[key] = value

        # Heuristic to convert dict to list if needed
        # (This is a very basic parser and might need more work for complex YAMLs)

    # Fixup: if a key has a dict value but all its children are list items (not possible with this logic)
    # Let's refine the list handling.

    # Actually, the original parser was almost working for lists if we just fixed the nesting.
    # Let's try another approach for load_config that's safer.
    return config

def load_config_v2(config_path="config.yaml"):
    """
    A slightly better simple YAML parser.
    """
    import yaml
    try:
        with open(config_path, 'r') as f:
            return yaml.safe_load(f)
    except (ImportError, Exception):
        # Fallback to a slightly better manual parser
        config = {}
        section = None
        subsection = None
        list_key = None

        try:
            with open(config_path, 'r') as f:
                for line in f:
                    indent = len(line) - len(line.lstrip())
                    sline = line.strip()
                    if not sline or sline.startswith('#'): continue

                    if indent == 0:
                        section = sline.split(':')[0].strip()
                        config[section] = {}
                        subsection = None
                        list_key = None
                    elif indent == 2:
                        parts = sline.split(':', 1)
                        key = parts[0].strip()
                        if len(parts) > 1 and parts[1].strip():
                            val = parts[1].strip().strip("'\"")
                            if val.isdigit(): val = int(val)
                            elif val.lower() == 'true': val = True
                            elif val.lower() == 'false': val = False
                            else:
                                try: val = float(val)
                                except: pass
                            config[section][key] = val
                            list_key = None
                            subsection = None
                        else:
                            # Could be start of list or dict
                            list_key = key
                            config[section][list_key] = [] # Assume list
                            subsection = key
                    elif indent == 4:
                        if sline.startswith('-'):
                            val = sline[1:].strip().strip("'\"")
                            if isinstance(config[section][list_key], list):
                                config[section][list_key].append(val)
                        else:
                            # It's a nested dict!
                            if isinstance(config[section][subsection], list):
                                config[section][subsection] = {} # Convert to dict
                            parts = sline.split(':', 1)
                            key = parts[0].strip()
                            val = parts[1].strip().strip("'\"")
                            if val.isdigit(): val = int(val)
                            elif val.lower() == 'true': val = True
                            elif val.lower() == 'false': val = False
                            else:
                                try: val = float(val)
                                except: pass
                            config[section][subsection][key] = val
            return config
        except Exception:
            return {}

# Replace the original load_config with the better one
load_config = load_config_v2

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
