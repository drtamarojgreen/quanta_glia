def load_prompts_from_file(filepath: str) -> dict[str, str]:
    prompts: dict[str, str] = {}
    current_key = None
    current_value: list[str] = []

    with open(filepath, "r", encoding="utf-8") as file_handle:
        for raw_line in file_handle:
            line = raw_line.strip()
            if not line or line.startswith("#"):
                continue

            if ":" in line and not raw_line.startswith(" "):
                if current_key:
                    prompts[current_key] = "\n".join(current_value).strip()
                key, value = line.split(":", 1)
                current_key = key.strip()
                current_value = [value.strip()]
            elif current_key:
                current_value.append(line)

    if current_key:
        prompts[current_key] = "\n".join(current_value).strip()

    return prompts
