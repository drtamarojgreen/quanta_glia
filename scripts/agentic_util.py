# quanta_glia/scripts/agentic_util.py

import logging
import os
from datetime import datetime

# Setup basic logging
def setup_logging(log_file=None):
    logging.basicConfig(
        level=logging.INFO,
        format='%(asctime)s - %(levelname)s - %(message)s',
        handlers=[
            logging.StreamHandler() # Log to console
        ]
    )
    if log_file:
        os.makedirs(os.path.dirname(log_file), exist_ok=True)
        file_handler = logging.FileHandler(log_file)
        file_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))
        logging.getLogger().addHandler(file_handler)

# Function to simulate LLM interaction
def call_llm(prompt, model, temperature, max_tokens):
    logging.info(f"Calling LLM ({model}) with prompt: {prompt[:100]}...")
    # In a real scenario, this would interact with an actual LLM API
    # For now, it returns a dummy response
    response = f"LLM response for: {prompt[:50]}... (Model: {model})"
    logging.info(f"LLM responded: {response[:50]}...")
    return response

# Function to save output to a file
def save_output(content, filename, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    filepath = os.path.join(output_dir, filename)
    with open(filepath, 'w') as f:
        f.write(content)
    logging.info(f"Output saved to {filepath}")
    return filepath

# Function to load content from a file
def load_content(filepath):
    if not os.path.exists(filepath):
        logging.error(f"File not found: {filepath}")
        return None
    with open(filepath, 'r') as f:
        content = f.read()
    logging.info(f"Content loaded from {filepath}")
    return content

# Function to generate a timestamped filename
def generate_timestamped_filename(base_name, extension):
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    return f"{base_name}_{timestamp}.{extension}"

# Function to load prompts from a text file
def load_prompts_from_file(filepath):
    prompts = {}
    current_key = None
    current_value = []

    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            if ':' in line and not line.startswith(' '): # New key-value pair
                if current_key:
                    prompts[current_key] = '\n'.join(current_value).strip()
                
                parts = line.split(':', 1)
                current_key = parts[0].strip()
                current_value = [parts[1].strip()]
            else: # Continuation of previous value
                if current_key:
                    current_value.append(line)
    
    if current_key:
        prompts[current_key] = '\n'.join(current_value).strip()

    return prompts