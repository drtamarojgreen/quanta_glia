#!/usr/bin/env python3

# Agentic Polling: A script to simulate agentic behavior with a local LLM
# Purpose: This script polls a local LLM server to generate, evaluate, and refine plans and prompts.

import os
import sys
import time
import json
import logging
import urllib.request
from pathlib import Path

# Add the project root to the Python path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import setup_logger, load_config

# Global config variables
LLAMACPP_URL = None
LLAMACPP_ENABLED = False
POLLING_INTERVAL_SECONDS = 60

def apply_config(config_data):
    """Applies the loaded configuration to global variables."""
    global LLAMACPP_URL, LLAMACPP_ENABLED, POLLING_INTERVAL_SECONDS

    if config_data is None:
        logging.error("No configuration data to apply.")
        sys.exit(1)

    # LLaMA.cpp settings
    llamacpp_config = config_data.get('llamacpp', {})
    LLAMACPP_URL = llamacpp_config.get("url", "http://localhost:8080/completion")
    LLAMACPP_ENABLED = llamacpp_config.get("enabled", False)

    # Agentic polling settings
    agentic_polling_config = config_data.get('agentic_polling', {})
    POLLING_INTERVAL_SECONDS = agentic_polling_config.get("interval_seconds", 60)

def main(config_path="config.yaml"):
    """Main function to run the agentic polling loop."""
    # Load and apply configuration
    config_data = load_config(config_path)
    apply_config(config_data)

    logger.info("Starting agentic polling script.")
    logger.info(f"LLaMA.cpp enabled: {LLAMACPP_ENABLED}")
    logger.info(f"Polling interval: {POLLING_INTERVAL_SECONDS} seconds")
    print("Agentic polling script started. Press Ctrl+C to exit.")

    if not LLAMACPP_ENABLED:
        logger.warning("LLaMA.cpp is not enabled. The script will run but not interact with the LLM.")

    while True:
        logger.info("--- Starting new agentic cycle ---")

        # 1. Generate a plan
        plan = generate_plan()

        # 2. Generate a prompt from the plan
        prompt = generate_prompt(plan)

        # 3. Evaluate the prompt
        evaluation = evaluate_prompt(prompt)

        # 4. Improve the prompt based on the evaluation
        improved_prompt = improve_prompt(prompt, evaluation)

        logger.info(f"Original plan: {plan}")
        logger.info(f"Generated prompt: {prompt}")
        logger.info(f"Evaluation: {evaluation}")
        logger.info(f"Improved prompt: {improved_prompt}")

        logger.info(f"--- Agentic cycle complete. Waiting for {POLLING_INTERVAL_SECONDS} seconds. ---")
        time.sleep(POLLING_INTERVAL_SECONDS)

def generate_plan():
    """Generates a high-level plan by querying the LLM."""
    logger.info("Generating a new plan...")
    prompt = "You are an autonomous agent. Generate a high-level plan to improve the functionality of this software repository."
    plan = query_llamacpp(prompt)
    return plan if plan else "Default plan: Analyze repository data."

def generate_prompt(plan):
    """Generates a specific prompt based on the plan by querying the LLM."""
    logger.info(f"Generating a prompt for the plan: '{plan}'")
    prompt = f"Based on the plan '{plan}', create a detailed, actionable prompt for another AI to execute."
    detailed_prompt = query_llamacpp(prompt, n_predict=256)
    return detailed_prompt if detailed_prompt else "Default prompt: Write a Python script."

def evaluate_prompt(prompt):
    """Evaluates the generated prompt by querying the LLM."""
    logger.info(f"Evaluating the prompt: '{prompt}'")
    evaluation_prompt = f"Please evaluate the following AI prompt for clarity, completeness, and effectiveness. Be critical and provide constructive feedback.\n\nPrompt: '{prompt}'"
    evaluation = query_llamacpp(evaluation_prompt)
    return evaluation if evaluation else "Default evaluation: The prompt is adequate."

def improve_prompt(prompt, evaluation):
    """Improves the prompt based on the evaluation by querying the LLM."""
    logger.info(f"Improving the prompt based on the evaluation: '{evaluation}'")
    improvement_prompt = f"Given the following prompt and its evaluation, rewrite the prompt to be better. Incorporate the feedback from the evaluation.\n\nOriginal Prompt: '{prompt}'\n\nEvaluation: '{evaluation}'\n\nImproved Prompt:"
    improved_prompt = query_llamacpp(improvement_prompt, n_predict=256)
    return improved_prompt if improved_prompt else prompt

def query_llamacpp(prompt_text, n_predict=128):
    """Sends a prompt to the LLaMA.cpp server and gets a completion."""
    if not LLAMACPP_ENABLED:
        logger.warning("LLaMA.cpp is not enabled. Skipping query.")
        return None

    logger.info("Sending prompt to LLaMA.cpp server.")

    data = {
        "prompt": prompt_text,
        "n_predict": n_predict
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
                completion = response_data.get("content", "").strip()
                logger.info("Successfully received completion from LLaMA.cpp.")
                return completion
            else:
                logger.error(f"LLaMA.cpp server returned status: {response.status}")
                return None
    except Exception as e:
        logger.error(f"Failed to connect to LLaMA.cpp server: {e}")
        return None

# Setup logging
logger = setup_logger(name='Agentic-Polling')

if __name__ == "__main__":
    try:
        config_file = "config.yaml"
        # Check for --config argument
        if "--config" in sys.argv:
            try:
                config_index = sys.argv.index("--config")
                config_file = sys.argv[config_index + 1]
            except (ValueError, IndexError):
                print("Error: --config flag must be followed by a file path.", file=sys.stderr)
                sys.exit(1)

        main(config_path=config_file)
    except KeyboardInterrupt:
        print("\nScript interrupted by user. Exiting.")
        sys.exit(0)
    except Exception as e:
        logger.error(f"An unexpected error occurred in main execution: {e}")
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
