import logging
from pathlib import Path
import sys

# Add project root to the Python path to allow for absolute imports
project_root = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(project_root))
from scripts.app.research_utils import connect_to_llm
# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

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

    llm_client = connect_to_llm()

    print("Generating ontology concepts using the LLM...")
    # Use a longer prediction length for this creative task
    concepts_text = llm_client.get_completion(prompt, n_predict=1024)

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
