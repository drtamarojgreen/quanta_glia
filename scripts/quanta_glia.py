# QuantaGlia: Repository Intelligence Harvester
# Purpose: Clone, scan, and distill meaningful information from repositories into structured knowledge

import os
import subprocess
import shutil
import logging
from datetime import datetime
from pathlib import Path

# Setup logging
logging.basicConfig(
    filename='quantaglia.log',
    filemode='a',
    level=logging.DEBUG,
    format='%(asctime)s [%(levelname)s] %(message)s'
)

# Configuration
KNOWLEDGE_BASE = Path("./knowledge_base")
REPO_CACHE = Path("./repo_cache")
TARGET_TOPICS = ["README", "LICENSE", "CONTRIBUTING", "ethics", "usage"]
MAX_REPOS = 10  # limit to avoid resource exhaustion

# Ensure directories exist
KNOWLEDGE_BASE.mkdir(exist_ok=True)
REPO_CACHE.mkdir(exist_ok=True)

def clone_repo(repo_url):
    repo_name = repo_url.split('/')[-1].replace('.git', '')
    dest_path = REPO_CACHE / repo_name
    if dest_path.exists():
        logging.info(f"Repo {repo_name} already cloned. Skipping.")
        return dest_path
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

def main(repo_urls):
    if len(repo_urls) > MAX_REPOS:
        logging.warning("Too many repositories. Truncating list.")
        repo_urls = repo_urls[:MAX_REPOS]
    for url in repo_urls:
        path = clone_repo(url)
        if path:
            extracted = extract_key_info(path)
            if extracted:
                store_to_knowledge_base(path.name, extracted)
    prune_cache()

# Example usage
if __name__ == "__main__":
    example_repos = [
        "https://github.com/example/project-one.git",
        "https://github.com/example/project-two.git"
    ]
    main(example_repos)
