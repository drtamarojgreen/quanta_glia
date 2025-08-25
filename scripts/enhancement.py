"""
This module handles parsing of enhancement documents.
"""

def parse_markdown_enhancements(file_path):
    """
    A simple parser to extract proposed enhancements from a markdown file.
    Looks for headers for modules and extracts checklist items.
    """
    enhancements = {}
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception:
        return {}

    current_module = None
    for line in lines:
        stripped_line = line.strip()
        if stripped_line.startswith('##') and 'Proposed Enhancements for' in stripped_line:
            current_module = stripped_line.replace('##', '').replace('Proposed Enhancements for', '').strip()
            enhancements[current_module] = []

        if current_module and stripped_line.startswith('- ['):
            enhancements[current_module].append(stripped_line.lstrip('- ').strip())

    return enhancements
