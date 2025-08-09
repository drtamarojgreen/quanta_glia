import sys
import json

def validate_action(action_string):
    """
    Simulates the QuantaEthos ethical governor.

    This mock version provides a simple validation logic:
    - Denies actions containing keywords like "critical" or "protected".
    - Approves all other actions.

    Args:
        action_string (str): A description of the action to be validated.

    Returns:
        dict: A dictionary containing the decision and reason.
    """
    decision = "approve"
    reason = "Action is within acceptable parameters."

    # Simple keyword-based check for critical actions
    if "critical" in action_string or "protected" in action_string:
        decision = "deny"
        reason = "High-risk command pattern detected on a protected path."

    return {
        "decision": decision,
        "reason": reason,
        "trust_score": 0.1 if decision == "deny" else 0.9
    }

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(json.dumps({
            "decision": "deny",
            "reason": "No action provided for validation.",
            "trust_score": 0.0
        }))
        sys.exit(1)

    action_to_validate = " ".join(sys.argv[1:])
    result = validate_action(action_to_validate)
    print(json.dumps(result, indent=2))
