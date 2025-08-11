#!/usr/bin/env python3

"""
research_semantics.py: Develops an ontology file from a given text source.
"""

import os
import sys
import json
import logging
import urllib.request
from pathlib import Path

# Add the project root to the Python path to allow for absolute imports
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))
from scripts.utils import load_config, setup_logger

# Setup logging
logger = setup_logger(name='ResearchSemantics')

# Global config variables
LLAMACPP_URL = None
LLAMACPP_ENABLED = False
ONTOLOGY_FILE = Path("scripts/app/ontology/base.txt")

def apply_config(config_data):
    """Applies the loaded configuration to global variables."""
    global LLAMACPP_URL, LLAMACPP_ENABLED

    if config_data is None:
        logging.error("No configuration data to apply.")
        sys.exit(1)

    llamacpp_config = config_data.get('llamacpp', {})
    LLAMACPP_URL = llamacpp_config.get("url", "http://localhost:8080/completion")
    LLAMACPP_ENABLED = llamacpp_config.get("enabled", False)

def develop_ontology_with_llamacpp(text_content):
    """Sends text to a LLaMA.cpp server to develop an ontology."""
    if not LLAMACPP_ENABLED:
        logger.error("LLaMA.cpp integration is not enabled in the configuration.")
        return None

    logger.info("Sending text to LLaMA.cpp for ontology development.")

    # Prepare the payload for the LLaMA.cpp server
    data = {
        "prompt": f"Please create a detailed ontology from the following text. The ontology should be in a structured format (e.g., RDF/XML, Turtle, or a simple indented list). \n\n{text_content}\n\nOntology:",
        "n_predict": 2048  # Allow for a longer response for the ontology
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
                ontology = response_data.get("content", "").strip()
                logger.info("Successfully received ontology from LLaMA.cpp.")
                return ontology
            else:
                logger.error(f"LLaMA.cpp server returned status: {response.status}")
                return None
    except Exception as e:
        logger.error(f"Failed to connect to LLaMA.cpp server: {e}")
        return None

def main(source_file="README.md"):
    """Main logic for developing and storing the ontology."""
    # Define path to config relative to this script's location to ensure it's found
    project_root = Path(__file__).resolve().parent.parent.parent
    config_file = project_root / "config.yaml"

    # Load and apply configuration
    config_data = load_config(config_file)
    apply_config(config_data)

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

    ontology = develop_ontology_with_llamacpp(source_text)

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
