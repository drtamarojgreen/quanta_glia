#!/usr/bin/env python3

# QuantaGlia: Repository Intelligence Harvester
# Purpose: Clone, scan, and distill meaningful information from repositories into structured knowledge

"""
QuantaGlia - Knowledge Harvester

This script is the primary component of the QuantaGlia system. Its current
functionality is to:
1.  Clone one or more Git repositories from provided URLs.
2.  Scan the cloned repositories for key files (e.g., README, LICENSE).
3.  Extract the content of these files and store them in a local "knowledge base"
    directory, organized by repository.

This script is the foundation for a more advanced system that will eventually
include intelligent pruning and curation of the knowledge base, as described in
the project documentation.

Usage:
    python3 scripts/quanta_glia.py <repo_url_1> <repo_url_2> ...

Example:
    python3 scripts/quanta_glia.py https://github.com/drtamarojgreen/quanta_ethos.git
    python3 scripts/quanta_glia.py ../quanta_ethos
"""

import os
import subprocess
import shutil
import logging
import sys
from datetime import datetime
from pathlib import Path
import json
import urllib.request

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config, clone_repo, prune_cache

# Setup logging
logging.basicConfig(
    filename='quantaglia.log',
    filemode='a',
    level=logging.DEBUG,
    format='%(asctime)s [%(levelname)s] %(message)s'
)

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

# Global config variables
KNOWLEDGE_BASE = None
REPO_CACHE = None
TARGET_TOPICS = None
MAX_REPOS = None
LLAMACPP_URL = None
LLAMACPP_ENABLED = False

def apply_config(config_data):
    """Applies the loaded configuration to global variables."""
    global KNOWLEDGE_BASE, REPO_CACHE, TARGET_TOPICS, MAX_REPOS, LLAMACPP_URL, LLAMACPP_ENABLED

    if config_data is None:
        logging.error("No configuration data to apply.")
        sys.exit(1)

    main_config = config_data.get('main', {})
    KNOWLEDGE_BASE = Path(main_config.get("knowledge_base", "./knowledge_base"))
    REPO_CACHE = Path(main_config.get("repo_cache", "./repo_cache"))
    TARGET_TOPICS = main_config.get("target_topics", ["README", "LICENSE", "CONTRIBUTING", "ethics", "usage"])
    MAX_REPOS = main_config.get("max_repos", 10)

    llamacpp_config = config_data.get('llamacpp', {})
    LLAMACPP_URL = llamacpp_config.get("url", "http://localhost:8080/completion")
    LLAMACPP_ENABLED = llamacpp_config.get("enabled", False)

# The KNOWLEDGE_BASE and REPO_CACHE directories are created inside main()
# after the configuration is loaded.

def clone_repo(repo_url):
    repo_name = repo_url.split('/')[-1].replace('.git', '')
    dest_path = REPO_CACHE / repo_name
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

def extract_key_info(repo_path):
    repo_name = repo_path.name
    extracted = {}
    for root, _, files in os.walk(repo_path):
        for file in files:
            if any(topic.lower() in file.lower() for topic in TARGET_TOPICS):
                full_path = Path(root) / file
                try:
                    with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    extracted[file] = content
                    logging.info(f"Extracted {file} from {repo_name}")
                except Exception as e:
                    logging.warning(f"Error reading {file} in {repo_name}: {e}")
    return extracted

def store_to_knowledge_base(repo_name, extracted_data):
    kb_dir = KNOWLEDGE_BASE / repo_name
    kb_dir.mkdir(parents=True, exist_ok=True)
    for fname, content in extracted_data.items():
        with open(kb_dir / fname, 'w', encoding='utf-8') as f:
            f.write(content)
    logging.info(f"Stored extracted data from {repo_name} to knowledge base.")

def prune_cache():
    for repo_dir in REPO_CACHE.iterdir():
        if repo_dir.is_dir():
            shutil.rmtree(repo_dir)
    logging.info("Pruned repo cache.")

def summarize_with_llamacpp(text_content):
    """Sends text to a LLaMA.cpp server for summarization."""
    if not LLAMACPP_ENABLED:
        return None

    logging.info("Sending text to LLaMA.cpp for summarization.")

    # Combine all extracted text into a single block
    full_text = "\n\n".join(text_content.values())

    # Prepare the payload for the LLaMA.cpp server
    data = {
        "prompt": f"Please summarize the following text from a software repository:\n\n{full_text}\n\nSummary:",
        "n_predict": 128  # Limit the summary length
    }

    req = urllib.request.Request(
        LLAMACPP_URL,
        data=json.dumps(data).encode('utf-8'),
        headers={'Content-Type': 'application/json'}
    )

    try:
        with urllib.request.urlopen(req) as response:
            if response.status == 200:
                response_data = json.loads(response.read().decode('utf-8'))
                summary = response_data.get("content", "").strip()
                logging.info("Successfully received summary from LLaMA.cpp.")
                return summary
            else:
                logging.error(f"LLaMA.cpp server returned status: {response.status}")
                return None
    except Exception as e:
        logging.error(f"Failed to connect to LLaMA.cpp server: {e}")
        return None

def main(repo_urls, config_path="config.yaml", summarize=False):
    """Main logic for cloning, extracting, and storing information."""
    # Load and apply configuration
    config_data = load_config(config_path)
    apply_config(config_data)

    # Command-line flag overrides config
    if summarize:
        global LLAMACPP_ENABLED
        LLAMACPP_ENABLED = True
        logging.info("Summarization enabled by command-line flag.")

    # Ensure directories exist (needs to be done after config is loaded)
    KNOWLEDGE_BASE.mkdir(exist_ok=True)
    REPO_CACHE.mkdir(exist_ok=True)

    if len(repo_urls) > MAX_REPOS:
        logging.warning("Too many repositories. Truncating list.")
        repo_urls = repo_urls[:MAX_REPOS]
    for url in repo_urls:
        path = clone_repo(url)
        if path:
            extracted = extract_key_info(path)
            if extracted:
                store_to_knowledge_base(path.name, extracted)
                summary = summarize_with_llamacpp(extracted)
                if summary:
                    summary_path = KNOWLEDGE_BASE / path.name / "summary.txt"
                    with open(summary_path, 'w', encoding='utf-8') as f:
                        f.write(summary)
                    logging.info(f"Stored summary for {path.name} to {summary_path}")

    prune_cache()

if __name__ == "__main__":
    try:
        args = sys.argv[1:]
        config_file = "config.yaml"

        # Check for --config argument
        if "--config" in args:
            try:
                config_index = args.index("--config")
                config_file = args[config_index + 1]
                # Remove the --config flag and its value from the list of args
                args.pop(config_index)
                args.pop(config_index)
            except (ValueError, IndexError):
                print("Error: --config flag must be followed by a file path.", file=sys.stderr)
                sys.exit(1)

        summarize_flag = False
        if "--summarize" in args:
            summarize_flag = True
            args.remove("--summarize")

        repo_urls = args
        if not repo_urls:
            # If no repo URLs are provided, run with the default for testing.
            logging.info("No repository URLs provided. Running with default test repository.")
            print("No repository URLs provided. Running with default test repository.")
            repo_urls = ["../quanta_ethos"]

        main(repo_urls, config_path=config_file, summarize=summarize_flag)

    except Exception as e:
        logging.error(f"An unexpected error occurred in main execution: {e}")
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
