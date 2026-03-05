import logging


def call_llm(prompt: str, model: str, temperature: float, max_tokens: int) -> str:
    logging.info("Calling LLM (%s) with prompt: %s...", model, prompt[:100])
    response = (
        f"LLM response for: {prompt[:50]}... "
        f"(Model: {model}, temperature: {temperature}, max_tokens: {max_tokens})"
    )
    logging.info("LLM responded: %s...", response[:80])
    return response
