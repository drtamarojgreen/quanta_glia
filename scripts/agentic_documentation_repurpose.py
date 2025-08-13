
#!/usr/bin/env python3

# Agentic Documentation Repurposing: A script to leverage a local LLM for documentation repurposing.
# Purpose: This script takes command-line parameters to guide a local LLM server in transforming existing documentation.

import os
import sys
import time
import json
import logging
import argparse

from pathlib import Path
from datetime import datetime, timezone

# Import from newly created modular files
from agentic_config import DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS, OUTPUT_DIR
from agentic_util import setup_logging, call_llm, save_output, load_content, generate_timestamped_filename

# Setup logging using the utility function
setup_logging(LOG_FILE)
logger = logging.getLogger(__name__)

class RepurposingAgent:
    def __init__(self, llm_model, llm_temperature, llm_max_tokens, output_dir):
        self.llm_model = llm_model
        self.llm_temperature = llm_temperature
        self.llm_max_tokens = llm_max_tokens
        self.output_dir = output_dir

    def _query_llm(self, prompt_text, n_predict=None):
        return call_llm(prompt_text, self.llm_model, self.llm_temperature, n_predict if n_predict else self.llm_max_tokens)

    def read_source_documents(self, source_dir="docs/agentic/"):
        source_path = Path(source_dir)
        if not source_path.exists():
            logger.warning(f"Source directory {source_dir} does not exist. No documents to read.")
            return {}

        documents = {}
        for md_file in source_path.glob("*.md"):
            try:
                documents[md_file.name] = md_file.read_text()
            except Exception as e:
                logger.error(f"Failed to read {md_file.name}: {e}")
        return documents

    def repurpose_document(self, doc_content, doc_type, purpose, topic):
        logger.info(f"Repurposing document for type='{doc_type}', purpose='{purpose}', topic='{topic}'.")
        prompt = f"""You are an AI assistant specialized in documentation repurposing. Your task is to transform the provided document content based on the specified type, purpose, and topic.

Document Type: {doc_type}
Purpose: {purpose}
Topic: {topic}

Original Document Content:
{doc_content}

Repurposed Document:"""
        repurposed_content = self._query_llm(prompt, n_predict=1024) # Increased n_predict for longer content
        return repurposed_content if repurposed_content else "No repurposed content generated."

    def save_repurposed_document(self, filename, content):
        save_output(content, filename, self.output_dir)

def main():
    parser = argparse.ArgumentParser(description="Repurpose existing documentation using LlamaCPP.")
    parser.add_argument("--type", default="general", help="Type of document (e.g., specification, implementation plan). Defaults to 'general'.")
    parser.add_argument("--purpose", default="overview", help="General theme to cover (e.g., integration, development). Defaults to 'overview'.")
    parser.add_argument("--topic", default="all", help="Specific topic (e.g., database schema for tissu, display modification for quanta_retina). Defaults to 'all'.")
    parser.add_argument("--source-dir", default="docs/agentic/", help="Directory containing source markdown documents.")
    parser.add_argument("--output-dir", default="docs/repurposed/", help="Directory to save repurposed markdown documents.")
    args = parser.parse_args()

    logger.info("Starting agentic documentation repurposing script.")
    logger.info(f"LLM Model: {DEFAULT_MODEL}")
    logger.info(f"Output directory: {args.output_dir}")

    agent = RepurposingAgent(DEFAULT_MODEL, TEMPERATURE, MAX_TOKENS, args.output_dir)

    # Read source documents
    source_documents = agent.read_source_documents(args.source_dir)
    if not source_documents:
        logger.error(f"No source documents found in {args.source_dir}. Exiting.")
        sys.exit(1)

    for filename, content in source_documents.items():
        repurposed_content = agent.repurpose_document(content, args.type, args.purpose, args.topic)
        new_filename = f"{Path(filename).stem}_{args.type}_{args.purpose}_{args.topic}"
        agent.save_repurposed_document(new_filename, repurposed_content)

    logger.info("Agentic documentation repurposing script finished.")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        logger.error(f"An unexpected error occurred in main execution: {e}")
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
