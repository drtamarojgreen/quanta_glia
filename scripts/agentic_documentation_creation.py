#!/usr/bin/env python3

# Agentic Documentation Creation: A script to leverage a local LLM for documentation generation.
# Purpose: This script collaborates with a local LLM server to generate rules, plans, vision, and enhancements documentation for quanta_* repositories.

import os
import sys
import time
import json
import logging

from pathlib import Path
from datetime import datetime, timezone

# Import from newly created modular files
from agentic_config import OUTPUT_DIR, LOG_FILE, DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS
from agentic_util import setup_logging, call_llm, save_output, load_content, generate_timestamped_filename

# Setup logging using the utility function
setup_logging(LOG_FILE)
logger = logging.getLogger(__name__)

class DocumentationAgent:
    def __init__(self, llm_model, llm_temperature, llm_max_tokens):
        self.llm_model = llm_model
        self.llm_temperature = llm_temperature
        self.llm_max_tokens = llm_max_tokens

    def _query_llm(self, prompt_text, n_predict=None):
        # Use the centralized call_llm from agentic_util
        return call_llm(prompt_text, self.llm_model, self.llm_temperature, n_predict if n_predict else self.llm_max_tokens)

    def generate_documentation_topic(self, current_topic, initial_rules):
        logger.info(f"Generating a new documentation topic based on: {current_topic}")
        prompt = f"You are an autonomous documentation agent for 'quanta_*' repositories. Your goal is to generate comprehensive documentation, including rules, planning, vision, and enhancements. Start with the basic rules: '{initial_rules}'. Based on the current focus: '{current_topic}', propose a new, specific documentation topic or rule to elaborate on. Be concise and direct, e.g., 'Rule: All code must be self-documenting.' or 'Planning: Future integration with external APIs.'"
        topic = self._query_llm(prompt, n_predict=128)
        return topic if topic else "Default Topic: General documentation."

    def generate_content(self, topic):
        logger.info(f"Generating content for topic: '{topic}'")
        prompt = f"Elaborate on the following documentation topic for 'quanta_*' repositories, providing detailed explanation, examples, or further rules. Focus on planning, vision, or enhancements. Topic: '{topic}'"
        content = self._query_llm(prompt, n_predict=512)
        return content if content else "Default Content: No content generated."

    def evaluate_content(self, content):
        logger.info(f"Evaluating the content: '{content[:100]}...' ")
        evaluation_prompt = f"""Please evaluate the following documentation content for clarity, completeness, adherence to the 'no external libraries, prioritize ecological harmony' rule, and overall usefulness for 'quanta_*' repositories. Provide constructive feedback.

Content: '{content}'"""
        evaluation = self._query_llm(evaluation_prompt)
        return evaluation if evaluation else "Default Evaluation: Content is adequate."

    def refine_content(self, content, evaluation):
        logger.info(f"Refining content based on evaluation: '{evaluation}'")
        improvement_prompt = f"""Given the following documentation content and its evaluation, rewrite or improve the content to address the feedback. Ensure it aligns with 'no external libraries, prioritize ecological harmony' for 'quanta_*' repositories.

Original Content: '{content}'

Evaluation: '{evaluation}'

Improved Content:"""
        refined_content = self._query_llm(improvement_prompt, n_predict=512)
        return refined_content if refined_content else content

def main():
    logger.info("Starting agentic documentation creation script.")
    logger.info(f"LLM Model: {DEFAULT_MODEL}")
    logger.info(f"Output directory: {OUTPUT_DIR}")
    print("Agentic documentation creation script started. Press Ctrl+C to exit.")

    agent = DocumentationAgent(DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS)

    initial_rules = "Basic rules: no external libraries, prioritize ecological harmony."
    current_topic = "Initial rules and planning for quanta_* repositories."

    while True:
        logger.info("--- Starting new agentic documentation cycle ---")

        # 1. Generate a documentation topic/rule/plan/vision/enhancement
        topic_content = agent.generate_documentation_topic(current_topic, initial_rules)

        # 2. Generate detailed content based on the topic
        generated_content = agent.generate_content(topic_content)

        # 3. Evaluate the generated content
        evaluation = agent.evaluate_content(generated_content)

        # 4. Refine the content based on the evaluation
        refined_content = agent.refine_content(generated_content, evaluation)

        # Save the refined content
        filename = generate_timestamped_filename("quanta_doc", "md")
        save_output(refined_content, filename, OUTPUT_DIR)

        logger.info(f"Generated Topic: {topic_content}")
        logger.info(f"Generated Content (first 100 chars): {generated_content[:100]}...")
        logger.info(f"Evaluation: {evaluation}")
        logger.info(f"Refined Content (first 100 chars): {refined_content[:100]}...")

        # Update current_topic for the next iteration (simple progression for now)
        POLLING_INTERVAL_SECONDS = 300 # Default to 5 minutes for documentation tasks
        current_topic = f"Further documentation on: {topic_content}. Consider previous rules: {initial_rules}"

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