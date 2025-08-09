import os
import subprocess
import shutil
import logging
from pathlib import Path

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s'
)

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
