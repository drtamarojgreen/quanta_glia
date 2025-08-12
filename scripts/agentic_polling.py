#!/usr/bin/env python3

# Agentic Polling: A script to simulate agentic behavior with a local LLM
# Purpose: This script polls a local LLM server to generate, evaluate, and refine plans and prompts.

import sys
import time
import json
import logging

import requests
from pathlib import Path
from datetime import datetime, timezone

# Add the project root to the Python path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from scripts.utils import load_config

# Global config variables
LLAMACPP_URL = None
LLAMACPP_ENABLED = False
MAX_RETRIES = 3
RETRY_DELAY = 5

def initialize_llamacpp():
    """Initializes LLaMA.cpp settings from config."""
    global LLAMACPP_URL, LLAMACPP_ENABLED
    config = load_config()
    LLAMACPP_ENABLED = config.get("llamacpp_enabled", False)
    if LLAMACPP_ENABLED:
        llamacpp_host = config.get("llamacpp_host", "127.0.0.1")
        llamacpp_port = config.get("llamacpp_port", 8080)
        LLAMACPP_URL = f"http://{llamacpp_host}:{llamacpp_port}/completion"
        logger.info(f"LLaMA.cpp enabled at {LLAMACPP_URL}")
    else:
        logger.info("LLaMA.cpp is disabled in the configuration.")

def get_base_prompt():
    """Reads the base prompt from a file."""
    try:
        with open("prompts/base_prompt.txt", "r") as f:
            return f.read()
    except FileNotFoundError:
        logger.error("Base prompt file not found.")
        return ""

def get_evaluation_prompt(prompt_to_evaluate):
    """Formats the evaluation prompt."""
    try:
        with open("prompts/evaluation_prompt.txt", "r") as f:
            evaluation_prompt_template = f.read()
        return evaluation_prompt_template.format(prompt=prompt_to_evaluate)
    except FileNotFoundError:
        logger.error("Evaluation prompt file not found.")
        return ""

def get_improvement_prompt(prompt, evaluation):
    """Formats the improvement prompt."""
    try:
        with open("prompts/improvement_prompt.txt", "r") as f:
            improvement_prompt_template = f.read()
        return improvement_prompt_template.format(prompt=prompt, evaluation=evaluation)
    except FileNotFoundError:
        logger.error("Improvement prompt file not found.")
        return ""

def evaluate_prompt(prompt):
    """Evaluates a given prompt using a meta-prompt."""
    evaluation_prompt = get_evaluation_prompt(prompt)
    if not evaluation_prompt:
        return "Evaluation failed: Could not generate evaluation prompt."

    logger.info("Evaluating prompt...")
    evaluation = query_llamacpp(evaluation_prompt, n_predict=128)
    return evaluation if evaluation else "Evaluation failed: No response from LLaMA.cpp."

def improve_prompt(prompt, evaluation):
    """Improves a given prompt based on evaluation."""
    improvement_prompt = get_improvement_prompt(prompt, evaluation)
    if not improvement_prompt:
        return prompt  # Return original prompt if improvement prompt fails

    logger.info("Improving prompt based on evaluation...")
    improved_prompt = query_llamacpp(improvement_prompt, n_predict=256)
    return improved_prompt if improved_prompt else prompt



class AgenticPollingJsonFormatter(logging.Formatter):
    """
    A custom logging formatter that outputs log records in a structured JSONL
    format for agentic polling events.
    """
    def format(self, record):
        log_object = {
            "timestamp": datetime.fromtimestamp(record.created, timezone.utc).isoformat() + "Z",
            "level": record.levelname,
            "message": record.getMessage(),
            "name": record.name,
            "filename": record.filename,
            "funcName": record.funcName,
            "lineno": record.lineno,
        }
        if hasattr(record, "event"):
            log_object["event"] = record.event
        if hasattr(record, "details"):
            log_object["details"] = record.details
        return json.dumps(log_object)

def setup_agentic_logger(log_path='logs/agentic.log', level=logging.INFO):
    """
    Sets up a dedicated logger for agentic polling that uses the AgenticPollingJsonFormatter.
    """
    logger = logging.getLogger("Agentic-Polling")
    logger.setLevel(level)
    logger.propagate = False

    if logger.hasHandlers():
        logger.handlers.clear()

    file_handler = logging.FileHandler(log_path, mode='a')
    file_handler.setFormatter(AgenticPollingJsonFormatter())
    logger.addHandler(file_handler)

    console_handler = logging.StreamHandler(sys.stderr)
    console_handler.setLevel(logging.ERROR)
    console_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
    logger.addHandler(console_handler)

    return logger

def query_llamacpp(prompt_text, n_predict=128):
    """Sends a prompt to the LLaMA.cpp server and gets a completion."""
    if not LLAMACPP_ENABLED:
        logger.warning("LLaMA.cpp is not enabled. Skipping query.")
        return None

    data = {
        "prompt": prompt_text,
        "n_predict": n_predict
    }

    logger.debug(f"Sending request to {LLAMACPP_URL} with data: {json.dumps(data)}")

    try:
        response = requests.post(LLAMACPP_URL, json=data)
        response.raise_for_status()  # Raise an exception for HTTP errors (4xx or 5xx)

        response_data = response.json()
        completion = response_data.get("content", "").strip()
        logger.debug(f"Received response from LLaMA.cpp: {json.dumps(response_data)}")
        logger.info("Successfully received completion from LLaMA.cpp.")
        return completion
    except requests.exceptions.RequestException as e:
        logger.error(f"Failed to connect to or get response from LLaMA.cpp server: {e}")
        return None
    except json.JSONDecodeError as e:
        logger.error(f"Failed to decode JSON response from LLaMA.cpp server: {e}")
        return None

# Setup logging
logger = setup_agentic_logger(log_path='logs/agentic.log')

if __name__ == "__main__":
    try:
        initialize_llamacpp()
        base_prompt = get_base_prompt()
        if not base_prompt:
            sys.exit(1)

        current_prompt = base_prompt
        iteration = 1
        max_iterations = 5  # To prevent infinite loops

        while iteration <= max_iterations:
            logger.info(f"--- Iteration {iteration} ---")
            logger.info(f"Current prompt:\n{current_prompt}")

            evaluation = evaluate_prompt(current_prompt)
            logger.info(f"Evaluation:\n{evaluation}")

            # Simple exit condition: if evaluation contains "good" or "effective"
            if "good" in evaluation.lower() or "effective" in evaluation.lower():
                logger.info("Prompt deemed effective. Halting improvement cycle.")
                break

            current_prompt = improve_prompt(current_prompt, evaluation)

            # Save the improved prompt to a file for inspection
            with open(f"prompts/improved_prompt_v{iteration}.txt", "w") as f:
                f.write(current_prompt)

            logger.info(f"Saved improved prompt to prompts/improved_prompt_v{iteration}.txt")

            iteration += 1
            time.sleep(2) # Pause between iterations

    except KeyboardInterrupt:
        logger.info("Process interrupted by user.")
        sys.exit(0)
