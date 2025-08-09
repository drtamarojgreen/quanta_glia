#!/usr/bin/env python3

# QuantaGlia Harvester
# Purpose: A focused script to clone repositories, extract key information,
# and process it using a local LLaMA.cpp server.

import os
import subprocess
import shutil
import logging
import sys
from pathlib import Path
import json
import urllib.request

# Setup logging
logging.basicConfig(
    level=logging.INFO,
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

def clone_repo(repo_url, cache_dir):
    """Clones a repository into a temporary cache directory."""
    repo_name = repo_url.split('/')[-1].replace('.git', '')
    dest_path = cache_dir / repo_name
    if dest_path.exists():
        logging.info(f"Repo {repo_name} already in cache. Using existing version.")
        return dest_path

    if os.path.isdir(repo_url):
        logging.info(f"Local directory detected at {repo_url}. Copying to cache.")
        try:
            shutil.copytree(repo_url, dest_path)
            logging.info(f"Copied local directory: {repo_url} to {dest_path}")
            return dest_path
        except Exception as e:
            logging.error(f"Failed to copy local directory {repo_url}: {e}")
            return None

    try:
        subprocess.run(["git", "clone", "--depth=1", repo_url, str(dest_path)], check=True, capture_output=True, text=True)
        logging.info(f"Cloned repo: {repo_url}")
        return dest_path
    except subprocess.CalledProcessError as e:
        logging.error(f"Failed to clone repo: {repo_url}. Error: {e.stderr}")
        return None

def extract_text_from_repo(repo_path, target_topics):
    """Extracts text content from files matching target topics."""
    extracted_content = {}
    for root, _, files in os.walk(repo_path):
        for file in files:
            if any(topic.lower() in file.lower() for topic in target_topics):
                full_path = Path(root) / file
                try:
                    with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    extracted_content[file] = content
                    logging.info(f"Extracted '{file}' from {repo_path.name}")
                except Exception as e:
                    logging.warning(f"Error reading {file} in {repo_path.name}: {e}")
    return extracted_content

def process_with_llamacpp(text_content, llamacpp_url):
    """Sends text to a LLaMA.cpp server for processing."""
    logging.info("Sending text to LLaMA.cpp for processing.")

    full_text = "\n\n".join(text_content.values())
    if not full_text.strip():
        logging.warning("No text content to process.")
        return None

    prompt = f"Please summarize the following content from a software repository:\n\n{full_text}\n\nSummary:"
    data = {
        "prompt": prompt,
        "n_predict": 256,
        "temperature": 0.2,
    }

    req = urllib.request.Request(
        llamacpp_url,
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
        logging.error(f"Failed to connect to LLaMA.cpp server at {llamacpp_url}: {e}")
        return None

def main():
    """Main logic for the harvester script."""
    # Load configuration
    config_data = load_config()
    if not config_data:
        print("Error: Could not load config.yaml. Please ensure it exists.", file=sys.stderr)
        sys.exit(1)

    main_config = config_data.get('main', {})
    llamacpp_config = config_data.get('llamacpp', {})

    repo_cache_dir = Path(main_config.get("repo_cache", "./repo_cache"))
    target_topics = main_config.get("target_topics", ["README"])
    llamacpp_url = llamacpp_config.get("url")
    llamacpp_enabled = llamacpp_config.get("enabled", False)

    if not llamacpp_enabled:
        print("LLaMA.cpp integration is disabled in config.yaml. Exiting.", file=sys.stderr)
        sys.exit(0)

    if not llamacpp_url:
        print("LLaMA.cpp URL is not configured in config.yaml. Exiting.", file=sys.stderr)
        sys.exit(1)

    # Get repo URLs from command-line arguments
    repo_urls = sys.argv[1:]
    if not repo_urls:
        print("Usage: python3 scripts/harvester.py <repo_url_1> <repo_url_2> ...", file=sys.stderr)
        sys.exit(1)

    # Create a temporary directory for clones
    repo_cache_dir.mkdir(exist_ok=True)

    for url in repo_urls:
        print(f"\n--- Processing Repository: {url} ---")
        repo_path = clone_repo(url, repo_cache_dir)
        if not repo_path:
            continue

        extracted_text = extract_text_from_repo(repo_path, target_topics)
        if not extracted_text:
            logging.warning(f"No target files found in {url}. Skipping.")
            continue

        summary = process_with_llamacpp(extracted_text, llamacpp_url)

        if summary:
            print("\n[LLaMA.cpp Summary]")
            print(summary)
        else:
            print("\n[No summary generated]")

    # Clean up the cache
    try:
        shutil.rmtree(repo_cache_dir)
        logging.info(f"Cleaned up cache directory: {repo_cache_dir}")
    except OSError as e:
        logging.error(f"Error removing cache directory {repo_cache_dir}: {e}")

if __name__ == "__main__":
    main()
