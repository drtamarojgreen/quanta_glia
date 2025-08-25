"""
This module handles parsing of planning phase documents.
"""
from pathlib import Path

def parse_markdown_phases(file_path):
    """
    A simple parser to extract phase-based checklist items from a markdown file.
    Looks for a header containing 'Phases' and extracts list items below it.
    """
    phases = {}
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception:
        return {}

    in_phases_section = False
    current_phase_header = None
    for line in lines:
        stripped_line = line.strip()
        # Check for a header that might start a phase section
        if stripped_line.startswith('##') and 'Phases' in stripped_line:
            in_phases_section = True
            continue

        # Check for a header that might be a specific phase
        if in_phases_section and stripped_line.startswith('###'):
            current_phase_header = stripped_line.replace('#', '').strip()
            phases[current_phase_header] = []

        # If we are in a phase section and have a phase header, look for list items
        if in_phases_section and current_phase_header and stripped_line.startswith('- '):
            phases[current_phase_header].append(stripped_line.lstrip('- ').strip())

    return phases
