import logging
import os
from datetime import datetime


def save_output(content: str, filename: str, output_dir: str) -> str:
    os.makedirs(output_dir, exist_ok=True)
    filepath = os.path.join(output_dir, filename)
    with open(filepath, "w", encoding="utf-8") as file_handle:
        file_handle.write(content)
    logging.info("Output saved to %s", filepath)
    return filepath


def load_content(filepath: str) -> str | None:
    if not os.path.exists(filepath):
        logging.error("File not found: %s", filepath)
        return None
    with open(filepath, "r", encoding="utf-8") as file_handle:
        content = file_handle.read()
    logging.info("Content loaded from %s", filepath)
    return content


def generate_timestamped_filename(base_name: str, extension: str) -> str:
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"{base_name}_{timestamp}.{extension}"
