

#!/usr/bin/env python3

# Agentic Polling: A script to simulate agentic behavior with a local LLM
# Purpose: This script polls a local LLM server to generate, evaluate, and refine plans and prompts.

import os
import sys
import time
import json
import logging

from pathlib import Path
from datetime import datetime, timezone

# Import from newly created modular files
from agentic_config import DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS, LOG_FILE
from agentic_util import setup_logging, call_llm, load_prompts_from_file

# Setup logging using the utility function
setup_logging(LOG_FILE)
logger = logging.getLogger(__name__)

class PollingAgent:
    def __init__(self, llm_model, llm_temperature, llm_max_tokens, prompts):
        self.llm_model = llm_model
        self.llm_temperature = llm_temperature
        self.llm_max_tokens = llm_max_tokens
        self.prompts = prompts

    def _query_llm(self, prompt_text, n_predict=None):
        return call_llm(prompt_text, self.llm_model, self.llm_temperature, n_predict if n_predict else self.llm_max_tokens)

    def generate_plan(self):
        logger.info("Generating a new plan...")
        prompt = self.prompts["INITIAL_PLAN_PROMPT"]
        plan = self._query_llm(prompt)
        return plan if plan else "Default plan: Analyze repository data."

    def generate_prompt(self, plan):
        logger.info(f"Generating a prompt for the plan: '{plan}'")
        prompt_template = self.prompts["PROMPT_GENERATION_PROMPT"]
        prompt = prompt_template.format(plan=plan)
        detailed_prompt = self._query_llm(prompt, n_predict=256)
        return detailed_prompt if detailed_prompt else "Default prompt: Write a Python script."

    def evaluate_prompt(self, prompt):
        logger.info(f"Evaluating the prompt: '{prompt}'")
        evaluation_prompt_template = self.prompts["PROMPT_EVALUATION_PROMPT"]
        evaluation_prompt = evaluation_prompt_template.format(prompt=prompt)
        evaluation = self._query_llm(evaluation_prompt)
        return evaluation if evaluation else "Default evaluation: The prompt is adequate."

    def improve_prompt(self, prompt, evaluation):
        logger.info(f"Improving the prompt based on the evaluation: '{evaluation}'")
        improvement_prompt_template = self.prompts["PROMPT_IMPROVEMENT_PROMPT"]
        improvement_prompt = improvement_prompt_template.format(prompt=prompt, evaluation=evaluation)
        improved_prompt = self._query_llm(improvement_prompt, n_predict=256)
        return improved_prompt if improved_prompt else prompt

def main():
    logger.info("Starting agentic polling script.")
    logger.info(f"LLM Model: {DEFAULT_MODEL}")
    logger.info(f"Log file: {LOG_FILE}")
    print("Agentic polling script started. Press Ctrl+C to exit.")

    # Load prompts from instructions.txt
    instructions_file = Path(__file__).parent / "instructions.txt"
    prompts = load_prompts_from_file(str(instructions_file))

    agent = PollingAgent(DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS, prompts)

    POLLING_INTERVAL_SECONDS = 60 # Default to 60 seconds

    while True:
        logger.info("--- Starting new agentic cycle ---")

        # 1. Generate a plan
        plan = agent.generate_plan()

        # 2. Generate a prompt from the plan
        prompt = agent.generate_prompt(plan)

        # 3. Evaluate the prompt
        evaluation = agent.evaluate_prompt(prompt)

        # 4. Improve the prompt based on the evaluation
        improved_prompt = agent.improve_prompt(prompt, evaluation)

        logger.info(f"Original plan: {plan}")
        logger.info(f"Generated prompt: {prompt}")
        logger.info(f"Evaluation: {evaluation}")
        logger.info(f"Improved prompt: {improved_prompt}")

        logger.info(f"--- Agentic cycle complete. Waiting for {POLLING_INTERVAL_SECONDS} seconds. ---")
        time.sleep(POLLING_INTERVAL_SECONDS)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nScript interrupted by user. Exiting.")
        sys.exit(0)
    except Exception as e:
        logger.error(f"An unexpected error occurred in main execution: {e}")
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
