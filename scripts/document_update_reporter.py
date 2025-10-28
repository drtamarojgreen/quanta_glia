
import os
import subprocess
from datetime import datetime, timedelta

def get_last_update_date(file_path):
    """
    Gets the last Git commit date for a given file.
    """
    try:
        cmd = ['git', 'log', '-1', '--format=%cI', file_path]
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return result.stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None

def find_document_files(directory="."):
    """
    Finds all document files (currently .md) in the given directory.
    """
    document_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".md"):
                document_files.append(os.path.join(root, file))
    return document_files

def generate_report(time_threshold_days=180):
    """
    Generates a report of documents that have not been updated recently.
    """
    now = datetime.now()
    threshold_date = now - timedelta(days=time_threshold_days)

    all_documents = find_document_files()
    outdated_documents = []
    updated_documents = []

    for doc in all_documents:
        last_update_str = get_last_update_date(doc)
        if last_update_str:
            last_update_date = datetime.fromisoformat(last_update_str)
            if last_update_date.replace(tzinfo=None) < threshold_date:
                outdated_documents.append((doc, last_update_date))
            else:
                updated_documents.append((doc, last_update_date))

    print("--- Document Update Report ---")
    print(f"Generated on: {now.strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"Threshold for outdated documents: {time_threshold_days} days")
    print("\\n--- Outdated Documents ---")
    if not outdated_documents:
        print("No outdated documents found.")
    else:
        for doc, update_date in outdated_documents:
            print(f"- {doc}: Last updated on {update_date.strftime('%Y-%m-%d')}")

    print("\\n--- Recently Updated Documents ---")
    if not updated_documents:
        print("No recently updated documents found.")
    else:
        for doc, update_date in updated_documents:
            print(f"- {doc}: Last updated on {update_date.strftime('%Y-%m-%d')}")

if __name__ == "__main__":
    generate_report()
