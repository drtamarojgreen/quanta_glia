import os
import subprocess
import shutil
import logging
import sys
import csv
from pathlib import Path

# Setup logging
logging.basicConfig(
    filename='reporting.log',
    filemode='a',
    level=logging.DEBUG,
    format='%(asctime)s [%(levelname)s] %(message)s'
)

REPO_CACHE = Path("./repo_cache")

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

def prune_cache():
    for repo_dir in REPO_CACHE.iterdir():
        if repo_dir.is_dir():
            shutil.rmtree(repo_dir)
    logging.info("Pruned repo cache.")

def main(repo_urls):
    """Main logic for cloning, analyzing, and reporting."""
    REPO_CACHE.mkdir(exist_ok=True)

    # In a real script, you'd loop through repo_urls
    # For now, let's process one for simplicity
    if not repo_urls:
        logging.error("No repository URLs provided.")
        sys.exit(1)

    repo_url = repo_urls[0]
    repo_path = clone_repo(repo_url)

    if repo_path:
        repo_name = repo_path.name
        repo_size = get_repo_size(repo_path)
        num_files, num_src_files, num_doc_files, num_test_files = analyze_repo_files(repo_path)

        logging.info(f"Repository: {repo_name}")
        logging.info(f"Size: {repo_size} bytes")
        logging.info(f"Total files: {num_files}")
        logging.info(f"Source files: {num_src_files}")
        logging.info(f"Doc files: {num_doc_files}")
        logging.info(f"Test files: {num_test_files}")

        report_data = {
            'repository_name': repo_name,
            'repository_size': repo_size,
            'number_files': num_files,
            'number_src_files': num_src_files,
            'number_doc_files': num_doc_files,
            'number_test_files': num_test_files,
        }
        write_to_csv(report_data)

    prune_cache()

def write_to_csv(data, filename="repository_data.csv"):
    file_exists = os.path.isfile(filename)
    with open(filename, 'a', newline='') as csvfile:
        fieldnames = ['repository_name', 'repository_size', 'number_files',
                      'number_src_files', 'number_doc_files', 'number_test_files']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

        if not file_exists:
            writer.writeheader()
        writer.writerow(data)
    logging.info(f"Report data written to {filename}")

def get_repo_size(repo_path):
    total_size = 0
    for dirpath, dirnames, filenames in os.walk(repo_path):
        for f in filenames:
            fp = os.path.join(dirpath, f)
            # skip if it is symbolic link
            if not os.path.islink(fp):
                total_size += os.path.getsize(fp)
    return total_size

def analyze_repo_files(repo_path):
    num_files = 0
    num_src_files = 0
    num_doc_files = 0
    num_test_files = 0

    src_exts = ['.py', '.js', '.java', '.c', '.cpp', '.go', '.rs', '.ts', '.html', '.css']
    doc_exts = ['.md', '.txt', '.rst']

    for dirpath, dirnames, filenames in os.walk(repo_path):
        # Exclude .git directory
        if '.git' in dirnames:
            dirnames.remove('.git')

        for f in filenames:
            num_files += 1
            file_path = Path(dirpath) / f
            file_ext = file_path.suffix
            file_stem = file_path.stem.lower()

            # Check for test files
            if 'test' in str(file_path).lower():
                num_test_files += 1
                continue

            # Check for doc files
            if file_ext in doc_exts or 'license' in file_stem or 'contributing' in file_stem:
                num_doc_files += 1
                continue

            # Check for src files
            if file_ext in src_exts:
                num_src_files += 1

    return num_files, num_src_files, num_doc_files, num_test_files

if __name__ == "__main__":
    if len(sys.argv) > 1:
        main(sys.argv[1:])
    else:
        print("Usage: python3 scripts/reporting.py <repo_url_1> <repo_url_2> ...", file=sys.stderr)
        sys.exit(1)
