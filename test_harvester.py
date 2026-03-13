#!/usr/bin/env python3
"""
Basic LLaMA.cpp API test
Purpose: Send a prompt to a running LLaMA.cpp server and print the response.
"""

import json
import urllib.request
import sys

LLAMACPP_URL = "http://localhost:8080/completion"  # Change to your LLaMA.cpp endpoint

def query_llamacpp(prompt):
    """Send prompt to LLaMA.cpp and return its output."""
    data = {
        "prompt": prompt,
        "n_predict": 128,
        "temperature": 0.2
    }

    req = urllib.request.Request(
        LLAMACPP_URL,
        data=json.dumps(data).encode("utf-8"),
        headers={"Content-Type": "application/json"}
    )

    try:
        with urllib.request.urlopen(req) as response:
            if response.status == 200:
                result = json.loads(response.read().decode("utf-8"))
                return result.get("content", "").strip()
            else:
                print(f"Server returned status: {response.status}", file=sys.stderr)
                return None
    except Exception as e:
        print(f"Error connecting to LLaMA.cpp: {e}", file=sys.stderr)
        return None

def main():
    prompt = "Write a one-sentence summary about the purpose of TissLang."
    print(f"Sending prompt to LLaMA.cpp: {prompt!r}")

    output = query_llamacpp(prompt)
    if output:
        print("\n--- LLaMA.cpp Response ---")
        print(output)
    else:
        print("\nNo output received.")

if __name__ == "__main__":
    main()
