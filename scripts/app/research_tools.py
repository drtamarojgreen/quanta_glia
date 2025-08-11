"""
This module provides tools for creating research topics and evaluation points
for the research integration framework. It includes a placeholder for connecting
to a Large Language Model (LLM).
"""
import json
import logging
import sys
import urllib.request
from typing import Any, Dict, List

# --- Configuration for LLaMA.cpp server ---
LLAMACPP_URL = "http://127.0.0.1:8080/completion"
LLAMACPP_ENABLED = True # Master switch

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

class LLMClient:
    """A client to get answers from a local LLaMA.cpp server."""
    def get_answer(self, topic: str) -> str:
        """Fetches an answer for a given research topic from the LLM."""
        if not LLAMACPP_ENABLED:
            logging.warning("LLaMA.cpp is not enabled. Returning placeholder answer.")
            return f"LLM is disabled. This is a placeholder answer about '{topic}'."

        logging.info(f"Sending prompt to LLaMA.cpp for topic: '{topic[:40]}...'")

        # Prepare the payload for the LLaMA.cpp server
        data = {
            "prompt": topic,
            "n_predict": 512, # Allow for a reasonably long answer
            "temperature": 0.7,
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
                    return f"Error: Failed to get response from LLM (status: {response.status})."
        except Exception as e:
            logging.error(f"Failed to connect to LLaMA.cpp server at {LLAMACPP_URL}: {e}")
            return f"Error: Could not connect to LLM: {e}"

def connect_to_llm() -> LLMClient:
    """
    Establishes a connection to the LLM.

    Returns:
        A client object for interacting with the LLM.
    """
    logging.info("Connecting to LLM...")
    return LLMClient()

# --- Research Topic and Evaluation Point Creation ---

def create_research_topic(concept: str) -> str:
    """
    Creates a standardized research topic question from a concept.

    Args:
        concept: The high-level concept to research (e.g., "TissLang").

    Returns:
        A formatted research topic string.
    """
    return f"Explain the concept of {concept} and provide two code examples."

def create_evaluation_points(topic: str) -> List[Dict[str, Any]]:
    """
    Generates a list of evaluation points for a given research topic.

    Note: This is a simplified generator. A real implementation might use an
    LLM to generate these points based on the topic.

    Args:
        topic: The research topic string.

    Returns:
        A list of evaluation point dictionaries.
    """
    try:
        # A more robust way to extract the concept from a topic like:
        # "Explain the concept of TissLang and provide two code examples."
        concept_name = topic.split(" of ", 1)[1].split(" and ", 1)[0].strip()
    except IndexError:
        logging.warning("Could not parse concept name from topic. Using 'concept' as a fallback.")
        concept_name = "concept"

    points = [
        {"text": f"The answer clearly defines {concept_name}'s purpose.",
         "type": "keyword",
         "params": {"keywords": [concept_name, "purpose", "language"], "min_count": 3}},
        {"text": "The answer provides at least two distinct code examples.",
         "type": "regex",
         "params": {"pattern": r"```(.*?)```.*?```(.*?)```"}},
        {"text": "The answer is at least 50 words long.",
         "type": "length",
         "params": {"min": 50}},
    ]
    logging.info(f"Generated {len(points)} evaluation points for topic: '{topic}'")
    return points

# --- Main execution block for demonstration ---

if __name__ == "__main__":
    # 1. Connect to the LLM (mock)
    llm_client = connect_to_llm()

    # 2. Define a concept and create a research topic
    concept = "Quantum Computing"
    topic = create_research_topic(concept)
    print(f"Research Topic: {topic}\n")

    # 3. Generate evaluation points for the topic
    evaluation_points = create_evaluation_points(topic)
    print("\nGenerated Evaluation Points:")
    for point in evaluation_points:
        print(f"  - {point['text']} (type: {point['type']})")
    print("\n")

    # 4. Get a (mock) answer from the LLM
    answer = llm_client.get_answer(topic)
    print(f"Mock LLM Answer:\n---\n{answer}\n---")

    # In a real workflow, you would now use the `evaluate_answer` function
    # from `scoring.py` to score this answer against the evaluation points.
    print("\nNext step would be to use scoring.py to evaluate the answer.")
