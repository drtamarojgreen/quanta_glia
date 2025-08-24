#!/usr/bin/env python3

"""
programming_semantics.py: Develops an ontology file from a given codebase.
"""

import os
import sys
import logging
from pathlib import Path

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))
from scripts.utils import load_config, setup_logger
from scripts.app.research_utils import connect_to_llm

# Setup logging
logger = setup_logger(name='ProgrammingSemantics')

ONTOLOGY_FILE = Path("scripts/app/ontology/programming_base.txt")

def main(source_directory="."):
    """Main logic for developing and storing the ontology."""
    # Define path to config relative to this script's location to ensure it's found
    project_root = Path(__file__).resolve().parent.parent.parent
    config_file = project_root / "config.yaml"

    # Load and apply configuration

    if not ONTOLOGY_FILE.parent.exists():
        ONTOLOGY_FILE.parent.mkdir(parents=True, exist_ok=True)
        logger.info(f"Created ontology directory: {ONTOLOGY_FILE.parent}")

    source_text = ""
    file_extensions = ['.py', '.md']
    logger.info(f"Scanning directory '{source_directory}' for files with extensions: {file_extensions}")

    for root, _, files in os.walk(source_directory):
        for file in files:
            if any(file.endswith(ext) for ext in file_extensions):
                filepath = Path(root) / file
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        source_text += f.read() + "\n\n"
                except Exception as e:
                    logger.warning(f"Could not read file {filepath}: {e}")

    if not source_text:
        logger.error("No source files found or read in the specified directory. Aborting.")
        sys.exit(1)

    llm_client = connect_to_llm()
    prompt = f"Please create a detailed semantic ontology of the following codebase. The ontology should identify key components, such as files, functions, classes, and their relationships. The output should be in a structured format (e.g., RDF/XML, Turtle, or a simple indented list), representing the architecture and logic of the code. \n\n{source_text}\n\nOntology:"

    logger.info("Sending codebase text to LLaMA.cpp for ontology development.")
    # Use a longer prediction length for this creative task
    ontology = llm_client.get_completion(prompt, n_predict=2048)

    if ontology:
        try:
            with open(ONTOLOGY_FILE, 'w', encoding='utf-8') as f:
                f.write(ontology)
            logger.info(f"Stored ontology to {ONTOLOGY_FILE}")
        except Exception as e:
            logger.error(f"Error writing ontology to {ONTOLOGY_FILE}: {e}")
            sys.exit(1)
    else:
        logger.error("Failed to develop ontology.")
        sys.exit(1)

if __name__ == "__main__":
    main()
