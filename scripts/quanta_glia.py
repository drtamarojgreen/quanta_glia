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
from scripts.utils import setup_logger, load_config, clone_repo, prune_cache
from scripts.audit import log_audit_event
from scripts.integration.quanta_tissu_connector import QuantaTissuConnector
from scripts.phase import parse_markdown_phases
from scripts.enhancement import parse_markdown_enhancements
import scripts.config as defaultConfig

import uuid

# Setup logging
logger = setup_logger(name='QuantaGlia-Harvester')

# Global config variables
KNOWLEDGE_BASE = None
REPO_CACHE = None
MAX_REPOS = None
LLAMACPP_URL = None
LLAMACPP_ENABLED = False

def apply_config(config_data):
    """Applies the loaded configuration to global variables."""
    global KNOWLEDGE_BASE, REPO_CACHE, MAX_REPOS, LLAMACPP_URL, LLAMACPP_ENABLED

    if config_data is None:
        logging.error("No configuration data to apply.")
        sys.exit(1)

    main_config = config_data.get('main', {})
    KNOWLEDGE_BASE = Path(main_config.get("knowledge_base", "./knowledge_base"))
    REPO_CACHE = Path(main_config.get("repo_cache", "./repo_cache"))
    MAX_REPOS = main_config.get("max_repos", 10)

    llamacpp_config = config_data.get('llamacpp', {})
    LLAMACPP_URL = llamacpp_config.get("url", "http://localhost:8080/completion")
    LLAMACPP_ENABLED = llamacpp_config.get("enabled", False)

# The KNOWLEDGE_BASE and REPO_CACHE directories are created inside main()
# after the configuration is loaded.


def analyze_repository(repo_path):
    """
    Analyzes a repository's structure, planning docs, and enhancement docs.
    """
    repo_name = repo_path.name
    knowledge = {
        'repo_name': repo_name,
        'key_components': [],
        'planning_phases': {},
        'enhancement_phases': {}
    }

    logging.info(f"Analyzing repository structure for {repo_name}")

    # 1. Capture Key Components (file structure)
    for root, dirs, files in os.walk(repo_path):
        # Create a relative path from the repo_path
        relative_root = Path(root).relative_to(repo_path)
        # Sort for consistent ordering
        dirs.sort()
        files.sort()
        for d in dirs:
            knowledge['key_components'].append(str(relative_root / d))
        for f in files:
            knowledge['key_components'].append(str(relative_root / f))

    logging.info(f"Found {len(knowledge['key_components'])} files and directories.")

    # 2. Find and parse planning and enhancement docs
    docs_path = repo_path / 'docs'
    if docs_path.is_dir():
        for doc_file in sorted(docs_path.iterdir()):
            if 'plan' in doc_file.name.lower():
                logging.info(f"Parsing planning document: {doc_file.name}")
                knowledge['planning_phases'].update(parse_markdown_phases(doc_file))
            if 'enhancement' in doc_file.name.lower():
                logging.info(f"Parsing enhancement document: {doc_file.name}")
                knowledge['enhancement_phases'].update(parse_markdown_enhancements(doc_file))

    return knowledge

def store_to_knowledge_base(repo_name, knowledge_data):
    """Stores the analyzed repository knowledge as a single JSON file."""
    kb_dir = KNOWLEDGE_BASE / repo_name
    kb_dir.mkdir(parents=True, exist_ok=True)

    output_path = kb_dir / 'repository_analysis.json'
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(knowledge_data, f, indent=4)
        logging.info(f"Stored repository analysis for {repo_name} to {output_path}")
    except Exception as e:
        logging.error(f"Failed to write knowledge base for {repo_name}: {e}")

def main(repo_urls, config_path="config.yaml", summarize=False, use_tissudb=False, tissudb_host=None, tissudb_port=None):
    """Main logic for cloning, analyzing, and storing information."""
    # Load and apply configuration
    config_data = load_config(config_path)
    apply_config(config_data)

    # Determine TissuDB configuration
    db_enabled = use_tissudb or defaultConfig.TISSDB_ENABLED
    db_host = tissudb_host or defaultConfig.TISSDB_HOST
    db_port = tissudb_port or defaultConfig.TISSDB_PORT

    # Ensure directories exist (needs to be done after config is loaded)
    KNOWLEDGE_BASE.mkdir(exist_ok=True)
    REPO_CACHE.mkdir(exist_ok=True)

    if len(repo_urls) > MAX_REPOS:
        logging.warning("Too many repositories. Truncating list.")
        repo_urls = repo_urls[:MAX_REPOS]

    connector = None
    if db_enabled:
        logging.info(f"TissuDB integration enabled. Connecting to {db_host}:{db_port}")
        connector = QuantaTissuConnector(db_host, db_port)
        connector.connect()

    try:
        for url in repo_urls:
            path = clone_repo(url, REPO_CACHE)
            if path:
                analysis_data = analyze_repository(path)
                if analysis_data:
                    # Store analysis to knowledge base
                    store_to_knowledge_base(path.name, analysis_data)

                    # Store to TissuDB if enabled
                    if connector:
                        logging.info(f"Storing knowledge for {path.name} in TissuDB.")
                        # Adapt data for TissuDB if necessary
                        tissu_data = analysis_data.copy()
                        tissu_data['timestamp'] = datetime.utcnow().isoformat()
                        connector.store_knowledge(tissu_data)
    finally:
        if connector:
            connector.disconnect()
        prune_cache(REPO_CACHE)


if __name__ == "__main__":
    try:
        args = sys.argv[1:]
        repo_urls = []
        config_file = "config.yaml"
        summarize_flag = False
        use_tissudb_flag = False
        tissudb_host_val = None
        tissudb_port_val = None

        i = 0
        while i < len(args):
            arg = args[i]
            if arg == "--config":
                if i + 1 < len(args):
                    config_file = args[i + 1]
                    i += 1
                else:
                    print("Error: --config flag must be followed by a file path.", file=sys.stderr)
                    sys.exit(1)
            elif arg == "--summarize":
                summarize_flag = True
            elif arg == "--use-tissdb":
                use_tissudb_flag = True
            elif arg == "--tissdb-host":
                if i + 1 < len(args):
                    tissudb_host_val = args[i + 1]
                    i += 1
                else:
                    print("Error: --tissdb-host flag must be followed by a value.", file=sys.stderr)
                    sys.exit(1)
            elif arg == "--tissdb-port":
                if i + 1 < len(args):
                    tissudb_port_val = args[i + 1]
                    i += 1
                else:
                    print("Error: --tissdb-port flag must be followed by a value.", file=sys.stderr)
                    sys.exit(1)
            else:
                repo_urls.append(arg)
            i += 1

        if not repo_urls:
            logging.info("No repository URLs provided. Running with default test repository.")
            print("No repository URLs provided. Running with default test repository.")
            repo_urls = ["../quanta_ethos"]

        main(
            repo_urls,
            config_path=config_file,
            summarize=summarize_flag,
            use_tissudb=use_tissudb_flag,
            tissudb_host=tissudb_host_val,
            tissudb_port=tissudb_port_val
        )

    except Exception as e:
        logging.error(f"An unexpected error occurred in main execution: {e}")
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
