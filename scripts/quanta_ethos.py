import sys
import json
import random

def validate_action(action_string: str) -> dict:
    """
    Simulates the QuantaEthos ethical governor using a weighted score model.

    This mock version provides a more nuanced validation logic based on the
    "Priority Policy Matrix" and "Decision Algorithm" from the ethical checkpoints.

    Args:
        action_string (str): A description of the action to be validated.

    Returns:
        dict: A dictionary containing the decision, reason, and supporting scores.
    """
    # Simulate scoring for each factor (0-100).
    # In a real system, these would come from complex evaluations.
    scores = {
        "ethical": random.uniform(50, 100),
        "privacy": random.uniform(70, 100),
        "resource": random.uniform(30, 90),
        "stability": random.uniform(60, 100),
    }

    # Simulate a high-risk scenario based on keywords
    is_high_risk = "critical" in action_string or "protected" in action_string
    if is_high_risk:
        scores["ethical"] = random.uniform(0, 30)
        scores["privacy"] = random.uniform(0, 50)

    # Apply weights from the Decision Algorithm.
    # The weights (0.95, 0.80, 0.10, 0.05) sum to 1.9. We normalize by dividing by this sum.
    final_score = (
        (scores["ethical"] * 0.95) +
        (scores["privacy"] * 0.80) +
        (scores["resource"] * 0.10) +
        (scores["stability"] * 0.05)
    ) / 1.9

    # Determine decision based on the final score
    if final_score >= 75:
        decision = "approve"
        reason = "Action approved with high confidence."
    elif final_score >= 40:
        decision = "uncertain"
        reason = f"Action requires human review. Score ({final_score:.1f}) is in the uncertainty threshold."
    else:
        decision = "deny"
        reason = f"Action denied. Score ({final_score:.1f}) is below the minimum threshold."

    if is_high_risk and decision != "approve":
        reason = f"High-risk command pattern detected. {reason}"

    return {
        "decision": decision,
        "reason": reason,
        "final_score": round(final_score, 2),
        "component_scores": {k: round(v, 2) for k, v in scores.items()},
    }

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(json.dumps({
            "decision": "deny",
            "reason": "No action provided for validation.",
            "final_score": 0,
        }, indent=2))
        sys.exit(1)

    action_to_validate = " ".join(sys.argv[1:])
    result = validate_action(action_to_validate)
    print(json.dumps(result, indent=2))
