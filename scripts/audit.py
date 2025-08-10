import json
from datetime import datetime

AUDIT_LOG_FILE = 'audit.log'

def log_audit_event(event_name, details):
    """
    Logs an audit event to the audit log file.

    :param event_name: The name of the event (e.g., 'repo_cloned').
    :param details: A dictionary containing event-specific details.
    """
    audit_entry = {
        'timestamp': datetime.utcnow().isoformat() + 'Z',
        'event_name': event_name,
        'details': details,
    }
    with open(AUDIT_LOG_FILE, 'a') as f:
        f.write(json.dumps(audit_entry) + '\n')
