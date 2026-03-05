import logging
from pathlib import Path

from .io import generate_timestamped_filename, save_output
from .llm import call_llm


def run_prompt_polling_cycle(model: str, temperature: float, max_tokens: int, prompts: dict[str, str]) -> dict[str, str]:
    plan = call_llm(prompts["INITIAL_PLAN_PROMPT"], model, temperature, max_tokens)

    generated_prompt = call_llm(
        prompts["PROMPT_GENERATION_PROMPT"].format(plan=plan),
        model,
        temperature,
        256,
    )
    evaluation = call_llm(
        prompts["PROMPT_EVALUATION_PROMPT"].format(prompt=generated_prompt),
        model,
        temperature,
        max_tokens,
    )
    improved_prompt = call_llm(
        prompts["PROMPT_IMPROVEMENT_PROMPT"].format(prompt=generated_prompt, evaluation=evaluation),
        model,
        temperature,
        256,
    )

    logging.info("Original plan: %s", plan)
    logging.info("Generated prompt: %s", generated_prompt)
    logging.info("Evaluation: %s", evaluation)
    logging.info("Improved prompt: %s", improved_prompt)

    return {
        "plan": plan,
        "prompt": generated_prompt,
        "evaluation": evaluation,
        "improved_prompt": improved_prompt,
    }


def run_documentation_creation_cycle(
    model: str,
    temperature: float,
    max_tokens: int,
    output_dir: str,
    current_topic: str,
    initial_rules: str,
) -> dict[str, str]:
    topic = call_llm(
        (
            "You are an autonomous documentation agent for 'quanta_*' repositories. "
            f"Basic rules: '{initial_rules}'. Current focus: '{current_topic}'."
        ),
        model,
        temperature,
        128,
    )
    content = call_llm(
        f"Elaborate on documentation topic for 'quanta_*' repositories: '{topic}'",
        model,
        temperature,
        512,
    )
    evaluation = call_llm(
        f"Evaluate this documentation content for clarity and completeness: '{content}'",
        model,
        temperature,
        max_tokens,
    )
    refined_content = call_llm(
        f"Improve documentation content '{content}' using feedback '{evaluation}'.",
        model,
        temperature,
        512,
    )

    filename = generate_timestamped_filename("quanta_doc", "md")
    output_path = save_output(refined_content, filename, output_dir)

    return {
        "topic": topic,
        "content": content,
        "evaluation": evaluation,
        "refined_content": refined_content,
        "output_path": output_path,
    }


def run_documentation_repurpose(
    model: str,
    temperature: float,
    max_tokens: int,
    source_dir: str,
    output_dir: str,
    doc_type: str,
    purpose: str,
    topic: str,
) -> list[str]:
    source_path = Path(source_dir)
    if not source_path.exists():
        raise FileNotFoundError(f"Source directory not found: {source_dir}")

    output_files: list[str] = []
    for markdown_file in source_path.glob("*.md"):
        content = markdown_file.read_text(encoding="utf-8")
        repurposed = call_llm(
            (
                "Transform the provided documentation. "
                f"Document Type: {doc_type}; Purpose: {purpose}; Topic: {topic}; "
                f"Original: {content}"
            ),
            model,
            temperature,
            max_tokens,
        )
        filename = f"{markdown_file.stem}_{doc_type}_{purpose}_{topic}.md"
        output_files.append(save_output(repurposed, filename, output_dir))

    return output_files
