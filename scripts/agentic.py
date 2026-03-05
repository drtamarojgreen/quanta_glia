#!/usr/bin/env python3

import argparse
import logging
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from agent_utils import (
    AgentConfig,
    load_prompts_from_file,
    run_documentation_creation_cycle,
    run_documentation_repurpose,
    run_prompt_polling_cycle,
    setup_logging,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Unified configurable agentic runner.")
    parser.add_argument("--mode", choices=["polling", "doc-create", "doc-repurpose"], required=True)
    parser.add_argument("--model", default="gemini-1.5-pro")
    parser.add_argument("--temperature", type=float, default=0.7)
    parser.add_argument("--max-tokens", type=int, default=2048)
    parser.add_argument("--log-file", default="./agent_logs/agent_activity.log")
    parser.add_argument("--output-dir", default="./agent_outputs")
    parser.add_argument("--interval-seconds", type=int, default=60)
    parser.add_argument("--iterations", type=int, default=1)

    parser.add_argument("--instructions-file", default="scripts/instructions.txt")

    parser.add_argument("--current-topic", default="Initial rules and planning for quanta_* repositories.")
    parser.add_argument("--initial-rules", default="Basic rules: no external libraries, prioritize ecological harmony.")

    parser.add_argument("--source-dir", default="docs/agentic/")
    parser.add_argument("--type", default="general", dest="doc_type")
    parser.add_argument("--purpose", default="overview")
    parser.add_argument("--topic", default="all")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    config = AgentConfig(
        model=args.model,
        temperature=args.temperature,
        max_tokens=args.max_tokens,
        log_file=args.log_file,
        output_dir=args.output_dir,
    )

    setup_logging(config.log_file)
    logger = logging.getLogger(__name__)
    logger.info("Starting unified agentic script in %s mode", args.mode)

    for iteration in range(1, args.iterations + 1):
        logger.info("--- Iteration %s ---", iteration)

        if args.mode == "polling":
            prompts = load_prompts_from_file(args.instructions_file)
            run_prompt_polling_cycle(config.model, config.temperature, config.max_tokens, prompts)

        elif args.mode == "doc-create":
            run_documentation_creation_cycle(
                config.model,
                config.temperature,
                config.max_tokens,
                config.output_dir,
                args.current_topic,
                args.initial_rules,
            )

        elif args.mode == "doc-repurpose":
            output_files = run_documentation_repurpose(
                config.model,
                config.temperature,
                config.max_tokens,
                args.source_dir,
                config.output_dir,
                args.doc_type,
                args.purpose,
                args.topic,
            )
            logger.info("Generated %s repurposed docs", len(output_files))

        if iteration < args.iterations:
            time.sleep(args.interval_seconds)

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except KeyboardInterrupt:
        print("\nScript interrupted by user. Exiting.")
        raise SystemExit(0)
