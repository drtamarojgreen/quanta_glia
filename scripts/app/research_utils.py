"""
Shared utilities for the research framework, including the LLM client.
"""
import json
import logging
import sys
import urllib.request
from typing import Optional
from pathlib import Path

# Add project root to the Python path to allow for absolute imports
project_root = Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(project_root))
from scripts.utils import load_config

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

class LLMClient:
    """A client to get answers from a local LLaMA.cpp server."""
    def __init__(self, url: str, enabled: bool):
        self.url = url
        self.enabled = enabled

    def get_completion(self, prompt: str, n_predict: int = 512) -> Optional[str]:
        """Fetches a completion for a given prompt from the LLM."""
        if not self.enabled:
            logging.warning("LLaMA.cpp is not enabled. Returning placeholder.")
            return f"LLM is disabled. This is a placeholder for prompt: '{prompt[:30]}...'"

        logging.info(f"Sending prompt to LLaMA.cpp: '{prompt[:40]}...'")

        data = {
            "prompt": prompt,
            "n_predict": n_predict,
            "temperature": 0.7,
        }

        req = urllib.request.Request(
            self.url,
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
            logging.error(f"Failed to connect to LLaMA.cpp server at {self.url}: {e}")
            return f"Error: Could not connect to LLM: {e}"

    def get_answer(self, topic: str) -> str:
        """Convenience method for getting a standard answer for a topic."""
        return self.get_completion(prompt=topic, n_predict=512)

def connect_to_llm() -> "LLMClient":
    """Establishes a connection to the LLM."""
    logging.info("Connecting to LLM...")

    # Load configuration from the central config file
    config_file = project_root / "config.yaml"
    config_data = load_config(config_file)

    if config_data and 'llamacpp' in config_data:
        llamacpp_config = config_data['llamacpp']
        url = llamacpp_config.get("url", "http://127.0.0.1:8080/completion")
        enabled = llamacpp_config.get("enabled", False)
    else:
        logging.warning("llamacpp section not found in config.yaml. Using defaults.")
        url = "http://127.0.0.1:8080/completion"
        enabled = False

    return LLMClient(url=url, enabled=enabled)