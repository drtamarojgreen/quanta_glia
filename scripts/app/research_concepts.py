import json
import logging
import urllib.request
from pathlib import Path
import sys

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

# Configuration for the LLaMA.cpp server
LLAMACPP_URL = "http://127.0.0.1:8080/completion"
LLAMACPP_ENABLED = True # Enable for this script

def generate_concepts_with_llamacpp(prompt_text):
    """Sends a prompt to a LLaMA.cpp server to generate text."""
    if not LLAMACPP_ENABLED:
        logging.warning("LLaMA.cpp is not enabled. Skipping concept generation.")
        return None

    logging.info("Sending prompt to LLaMA.cpp server.")

    # Prepare the payload for the LLaMA.cpp server
    data = {
        "prompt": prompt_text,
        "n_predict": 512  # Allow for a longer response for concepts
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
                content = response_data.get("content", "").strip()
                logging.info("Successfully received response from LLaMA.cpp.")
                return content
            else:
                logging.error(f"LLaMA.cpp server returned status: {response.status}")
                return None
    except Exception as e:
        logging.error(f"Failed to connect to LLaMA.cpp server: {e}")
        return None

def main():
    """
    Main function to generate and save ontology concepts.
    """
    output_dir = Path("scripts/app/ontology")
    output_file = output_dir / "concepts.txt"

    # Create the output directory if it doesn't exist
    try:
        output_dir.mkdir(parents=True, exist_ok=True)
    except OSError as e:
        logging.error(f"Failed to create directory {output_dir}: {e}")
        sys.exit(1)

    # Define the prompt for the LLM
    prompt = (
        "Generate a hierarchical list of fundamental concepts for a "
        "personal knowledge management and research augmentation system. "
        "The system is called 'QuantaGlia'. The concepts should cover topics like: "
        "knowledge representation, data sources, automated analysis, user interaction, "
        "and ethical considerations. Present it as a simple text-based hierarchy."
        "\n\nExample format:\n1. Core Concepts\n  1.1. Knowledge Node\n  1.2. Semantic Link\n2. Data Ingestion\n  2.1. Web Scraper\n\nOntology:"
    )

    print("Generating ontology concepts using the LLM...")
    concepts_text = generate_concepts_with_llamacpp(prompt)

    if concepts_text:
        try:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(concepts_text)
            print(f"Successfully wrote concepts to {output_file}")
            logging.info(f"Ontology concepts saved to {output_file}")
        except IOError as e:
            logging.error(f"Failed to write to file {output_file}: {e}")
            sys.exit(1)
    else:
        print("Failed to generate concepts from the LLM.")
        logging.error("No concepts were generated, so the output file was not created.")
        sys.exit(1)

if __name__ == "__main__":
    main()
