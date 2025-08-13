
# quanta_glia/scripts/agentic_config.py

# Configuration for agentic scripts

# --- General Settings ---
OUTPUT_DIR = "./agent_outputs"
LOG_FILE = "./agent_logs/agent_activity.log"

# --- LLM Settings ---
DEFAULT_MODEL = "gemini-1.5-pro"
TEMPERATURE = 0.7
MAX_TOKENS = 2048

# --- Documentation Creation Settings ---
DOC_TEMPLATE_PATH = "./templates/doc_template.md"

# --- Evaluation Settings ---
EVAL_DATA_PATH = "./evaluation_data/eval_set.json"

# --- Onboarding Settings ---
ONBOARDING_GUIDE_PATH = "./onboarding_guides/new_dev_guide.md"
