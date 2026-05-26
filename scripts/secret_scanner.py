#!/usr/bin/env python3
import os
import re
import sys

# Simple regex-based secret scanner
SECRET_PATTERNS = {
    "AWS Access Key": r"AKIA[0-9A-Z]{16}",
    "Generic Secret/Password": r"(?i)(password|secret|passwd|api_key|apikey|token)[\s:=]+['\"]([a-zA-Z0-9_\-\.]{8,})['\"]",
    "Private Key": r"-----BEGIN [A-Z ]+ PRIVATE KEY-----",
}

def scan_file(filepath):
    findings = []
    try:
        with open(filepath, 'r', errors='ignore') as f:
            for i, line in enumerate(f, 1):
                for name, pattern in SECRET_PATTERNS.items():
                    if re.search(pattern, line):
                        findings.append((i, name))
    except Exception:
        return []
    return findings

def main():
    root_dir = "."
    if len(sys.argv) > 1:
        root_dir = sys.argv[1]

    found_any = False
    print(f"Scanning {root_dir} for secrets...")

    ignored_dirs = {".git", "build", ".objs", "node_modules", "__pycache__"}

    for root, dirs, files in os.walk(root_dir):
        dirs[:] = [d for d in dirs if d not in ignored_dirs]
        for file in files:
            path = os.path.join(root, file)
            findings = scan_file(path)
            if findings:
                found_any = True
                for line_no, secret_type in findings:
                    print(f"FOUND {secret_type} in {path}:{line_no}")

    if found_any:
        print("Secrets scan failed.")
        sys.exit(1)
    else:
        print("No secrets detected.")
        sys.exit(0)

if __name__ == "__main__":
    main()
