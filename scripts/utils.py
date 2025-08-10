import os
import subprocess
import shutil
import logging
from pathlib import Path
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
