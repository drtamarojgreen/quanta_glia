from dataclasses import dataclass


@dataclass
class AgentConfig:
    model: str = "gemini-1.5-pro"
    temperature: float = 0.7
    max_tokens: int = 2048
    log_file: str = "./agent_logs/agent_activity.log"
    output_dir: str = "./agent_outputs"
