# Phase V Implementation Guide

This document provides a detailed, developer-focused guide on how to implement the future enhancements listed in `scripts/app/README.md`. Each section outlines the purpose of a feature, the files to modify, the specific code changes required, and usage examples.

---

## 1. Performance Logging

**Purpose**: Augment evaluation results to include the execution time for each check, helping to identify performance bottlenecks.

**Files to Modify**:
*   `evaluators.py`: To measure the time taken for each check.
*   `scoring.py`: To handle the new performance data in the results.

**Implementation Steps**:
1.  **Modify `evaluators.py`**:
    *   Import the `time` module at the top of the file.
    *   In `run_evaluation_point`, wrap the core validation logic to measure execution time.
    *   Change the return type from a `(bool, str)` tuple to a dictionary containing the result, note, and duration.

    ```python
    # In evaluators.py
    import time

    def run_evaluation_point(point, answer):
        # ... (existing setup code) ...
        start_time = time.time()

        # ... (existing if/elif block for validators) ...

        duration_ms = (time.time() - start_time) * 1000

        # The 'result' and 'note' variables are set inside the if/elif block
        return {"ok": result, "note": note, "duration_ms": duration_ms}
    ```

2.  **Update `scoring.py`**:
    *   In `evaluate_answer`, update the call to `run_evaluation_point` and the subsequent processing to handle the new dictionary return type.

    ```python
    # In scoring.py
    def evaluate_answer(answer, evaluation_points):
        # ...
        details = []
        for point in evaluation_points:
            # result_data is now a dictionary
            result_data = run_evaluation_point(point, answer)

            if result_data["ok"]:
                passing_checks += 1

            details.append({
                "point": point["text"],
                **result_data # Unpack the dict here
            })
        # ...
        return score, details
    ```

**Usage Example**:
The `details` object returned by `evaluate_answer` will now automatically contain the performance metric. No change is needed in the calling code, but the output data will be richer.

```json
{
    "point": "Mentions the Transformer model.",
    "ok": true,
    "note": "Found keyword 'Transformer'.",
    "duration_ms": 2.5
}
```

---

## 2. Report Generation

**Purpose**: Export evaluation results into simple, portable formats like CSV or Markdown for analysis and sharing.

**Files to Modify**:
*   `reporting.py`: (New file) To contain the report generation logic.
*   `research_integration.py`: To demonstrate how to use the new reporting functions.

**Implementation Steps**:
1.  **Create `reporting.py`**:
    *   Create a new file `scripts/app/reporting.py`.
    *   Import the `csv` module.
    *   Implement functions to generate reports.

    ```python
    # In scripts/app/reporting.py
    import csv

    def generate_csv_report(score_details, output_path):
        """Exports evaluation results to a CSV file."""
        headers = ["model_index", "final_score", "check_text", "status", "note", "duration_ms"]
        with open(output_path, "w", newline="") as f:
            writer = csv.DictWriter(f, fieldnames=headers)
            writer.writeheader()
            for i, (score, details) in enumerate(score_details):
                for result in details:
                    writer.writerow({
                        "model_index": i,
                        "final_score": score,
                        "check_text": result["point"],
                        "status": "PASS" if result["ok"] else "FAIL",
                        "note": result.get("note", ""),
                        "duration_ms": result.get("duration_ms", -1)
                    })

    def generate_markdown_report(score_details, output_path):
        """Exports evaluation results to a Markdown file."""
        # ... (implementation for Markdown table generation) ...
    ```

2.  **Update `research_integration.py`**:
    *   Import the new functions and add an example of their use at the end of the main execution block.

**Usage Example**:
```python
# In research_integration.py
from reporting import generate_csv_report

# ... (after compare_answers is called) ...
generate_csv_report(score_details, "evaluation_report.csv")
```

---

## 3. Weighted Scoring

**Purpose**: Allow critical checks to have a greater impact on the final score.

**Files to Modify**:
*   `scoring.py`: To implement the weighted scoring logic.

**Implementation Steps**:
1.  **Modify `evaluate_answer` in `scoring.py`**:
    *   Update the scoring logic to account for an optional `weight` parameter in each evaluation point.
    *   The final score becomes the sum of weights of passed checks divided by the sum of all weights.

    ```python
    # In scoring.py
    def evaluate_answer(answer, evaluation_points):
        passing_score = 0
        total_weight = 0
        details = []

        for point in evaluation_points:
            weight = point.get("weight", 1.0)
            total_weight += weight

            result_data = run_evaluation_point(point, answer)
            if result_data["ok"]:
                passing_score += weight

            details.append({
                "point": point["text"],
                **result_data
            })

        score = passing_score / total_weight if total_weight > 0 else 0
        return score, details
    ```

**Usage Example**:
```python
evaluation_points = [
    {"text": "Crucial: Mentions Transformer.", "type": "keyword", "params": {"keywords": ["Transformer"]}, "weight": 3.0},
    {"text": "Minor: Contains at least 50 words.", "type": "length", "params": {"min": 50}, "weight": 0.5}
]
```

---

## 4. Conditional Evaluation

**Purpose**: Enable dependent, tree-like evaluation logic where a check only runs if its dependency passed.

**Files to Modify**:
*   `scoring.py`: To manage the dependency logic.

**Implementation Steps**:
1.  **Modify `evaluate_answer` in `scoring.py`**:
    *   Add logic to handle `id` and `depends_on` fields in evaluation points.
    *   Process checks in a way that respects dependencies. A simple loop is sufficient if dependencies always refer to points earlier in the list.
    *   Maintain a dictionary of results to check dependency status.

    ```python
    # In scoring.py
    def evaluate_answer(answer, evaluation_points):
        # ...
        results_by_id = {}
        details = []

        for point in evaluation_points:
            dependency_id = point.get("depends_on")
            should_run = True
            note_override = None

            if dependency_id:
                if dependency_id not in results_by_id or not results_by_id[dependency_id]["ok"]:
                    should_run = False
                    note_override = f"Skipped; dependency '{dependency_id}' did not pass."

            if should_run:
                result_data = run_evaluation_point(point, answer)
            else:
                result_data = {"ok": False, "note": note_override}

            point_id = point.get("id")
            if point_id:
                results_by_id[point_id] = result_data

            # ... (update scoring and details list as before) ...
    ```

**Usage Example**:
```python
evaluation_points = [
    {"id": "has_json", "text": "Answer contains a JSON block.", "type": "regex", "params": {"pattern": "```json"}},
    {"id": "is_valid_json", "text": "The JSON block is valid.", "type": "json_schema", "params": {"schema": {}}, "depends_on": "has_json"}
]
```

---

## 5. Negation Validator

**Purpose**: Add a validator that passes only if a specified list of keywords is *not* found.

**Files to Modify**:
*   `evaluators.py`: To add the new validator logic.

**Implementation Steps**:
1.  **Modify `run_evaluation_point` in `evaluators.py`**:
    *   Add a new `elif` block for the `negation` type.
    *   The logic should check for the presence of any of the specified keywords.
    *   The check passes if *none* are found.

    ```python
    # In evaluators.py
    # ... inside run_evaluation_point
    elif t == 'negation':
        keywords = p.get('keywords', [])
        found_keywords = [k for k in keywords if k.lower() in answer.lower()]
        if not found_keywords:
            result = True
            note = "No prohibited keywords found."
        else:
            result = False
            note = f"Prohibited keywords found: {', '.join(found_keywords)}"
    ```

**Usage Example**:
```python
evaluation_points = [
    {"text": "Avoids using weakeners.", "type": "negation", "params": {"keywords": ["maybe", "perhaps", "possibly"]}}
]
```

---

... (This document would continue in this format for all 20 enhancements) ...

---

## 20. Validator Chaining

**Purpose**: Enable complex, multi-step validations where the output of one validator can be piped as the input to another.

**Files to Modify**:
*   `evaluators.py`: To handle the chaining logic.
*   `scoring.py`: To orchestrate the chained execution.

**Implementation Steps**:
1.  **Standardize Validator Output**:
    *   Modify all validators in `run_evaluation_point` to not only return `(bool, str)` but also a potential `output` value. For most, this will be `None`. For a `regex` validator with capturing groups, it could be the captured string.
    *   The return value from `run_evaluation_point` could become a dictionary: `{"ok": bool, "note": str, "output": any}`.

2.  **Modify `scoring.py`**:
    *   In `evaluate_answer`, introduce a new evaluation point `type`, perhaps `chain`.
    *   A `chain` point would have a `steps` parameter, which is a list of standard evaluation points.
    *   The `evaluate_answer` function would iterate through the steps, passing the `output` of step `N` as the `answer` to step `N+1`.
    *   The result of the chain is the result of its final step.

**Usage Example**:
```python
evaluation_points = [
    {
        "text": "Extracts and validates the user profile from the JSON.",
        "type": "chain",
        "steps": [
            {
                "type": "regex",
                "params": {"pattern": "```json\n(.*)\n```", "capture": 1},
                "note": "Extract JSON block from markdown."
            },
            {
                "type": "json_schema",
                "params": {"schema": {"type": "object", "properties": {"name": {"type": "string"}}}},
                "note": "Validate the extracted JSON."
            }
        ]
    }
]
```
