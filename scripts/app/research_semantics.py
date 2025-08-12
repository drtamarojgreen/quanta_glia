#!/usr/bin/env python3

"""
research_semantics.py: Develops an ontology file from a given text source.
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
logger = setup_logger(name='ResearchSemantics')

ONTOLOGY_FILE = Path("scripts/app/ontology/base.txt")

def main(source_file="README.md"):
    """Main logic for developing and storing the ontology."""
    # Define path to config relative to this script's location to ensure it's found
    project_root = Path(__file__).resolve().parent.parent.parent
    config_file = project_root / "config.yaml"

    # Load and apply configuration

    if not ONTOLOGY_FILE.parent.exists():
        logger.error(f"Ontology directory not found: {ONTOLOGY_FILE.parent}")
        sys.exit(1)

    try:
        with open(source_file, 'r', encoding='utf-8') as f:
            source_text = f.read()
    except FileNotFoundError:
        logger.error(f"Source file not found: {source_file}")
        sys.exit(1)
    except Exception as e:
        logger.error(f"Error reading source file {source_file}: {e}")
        sys.exit(1)

    llm_client = connect_to_llm()
    prompt = f"Please create a detailed ontology from the following text. The ontology should be in a structured format (e.g., RDF/XML, Turtle, or a simple indented list). \n\n{source_text}\n\nOntology:"

    logger.info("Sending text to LLaMA.cpp for ontology development.")
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
