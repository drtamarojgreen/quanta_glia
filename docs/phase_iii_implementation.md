# Phase III Implementation Plan: Live Integration & Advanced Intelligence

This document outlines the implementation plan for Phase III of the QuantaGlia-Pruner module. This phase marks a significant milestone: transitioning the pruner from a component with simulated interactions to a fully integrated member of the PrismQuanta ecosystem. It will replace mocked data with live data from other Quanta services and introduce more advanced analytical capabilities.

---

## 1. Core Objective

The primary goal of Phase III is to achieve **live, end-to-end integration** with `QuantaEthos` and `QuantaSensa`. This will ground the pruner's decisions in real ethical policies and usage metrics. Additionally, this phase will introduce a foundational dependency analysis capability to prevent the accidental pruning of critical shared libraries.

---

## 2. Key Features

Phase III will deliver the following key features:


-   **Advanced Repository Metrics:** The pruner will be enhanced to perform a deep scan of each repository to gather a rich set of metrics, replacing the simple mocked scores. This data will provide a more holistic view of a repository's value and will include:
    -   Total repository size
    -   Counts of files, directories, source files, test files, and documentation files
    -   Predominant programming language
    -   Date of the last edit

-   **Human-in-the-Loop (HITL) Workflow:** The `REVIEW` decision will be fully implemented. When a repository is flagged for review, the pruner will write the `PruneDecision` object to a dedicated review queue (e.g., a JSONL file like `review_queue.log`) for manual operator assessment.

-   **Dependency Analysis (Python):** The pruner will be enhanced to parse `requirements.txt` files within repositories. Repositories identified as dependencies by others in the knowledge base will be protected from pruning, addressing a key risk outlined in the project plan.

-   **LLM-Powered Ethical Integration:** The mocked `get_ethical_verdict` function will be replaced with a client that communicates with a local **LLaMA.cpp instance**. The pruner will formulate a detailed prompt, asking the LLM to act as an ethical governor and provide a verdict on the proposed action.

---

## 3. API Contracts & Local Analysis

To facilitate integration, the following conceptual API contracts will be implemented.

### LLaMA.cpp for Ethical Verdict (`/completion`)

Instead of a dedicated microservice, the pruner will leverage a local LLaMA.cpp instance for ethical reasoning.

**Request (Pruner -> LLaMA.cpp):**
The `prompt` will be a carefully constructed request asking the LLM to act as an ethical governor.
```json
{
  "prompt": "You are QuantaEthos, an ethical governor for an autonomous software system. Your role is to approve or deny actions based on system safety and knowledge preservation. A sub-module, QuantaGlia-Pruner, proposes the following action:\n\n- **Action:** DELETE\n- **Target:** 'old-project-alpha'\n- **Justification:** 'Score (0.92) meets or exceeds DELETE threshold (0.9)'\n- **Metrics:**\n  - Repository Size: 15.2 MB\n  - Total Files: 150 (Source: 80, Tests: 40, Docs: 10)\n  - Language: Python\n  - Last Edit: 450 days ago\n\nBased on this information, is the proposed action safe and reasonable? Respond with only 'VERDICT: ALLOW' or 'VERDICT: DENY' followed by a brief reason.",
  "n_predict": 64
}
```

**Response (Ethos -> Pruner):**
```json
{
  "verdict": "ALLOW",
  "reason": "Action aligns with policy PQ-PRUNE-001.",
  "confidence": 0.98
}
```

### QuantaSensa (`/v1/metrics/usage_score`)

**Request (Pruner -> Sensa):**
`GET /v1/metrics/usage_score?repo_name=old-project-alpha`

**Response (Sensa -> Pruner):**
```json
{
  "repo_name": "old-project-alpha",
  "usage_score": 0.01,
  "last_accessed": "2023-01-15T12:00:00Z"
}
```

---

## 4. Configuration (`config.yaml`)

The `config.yaml` file will be updated to include the service endpoints for the live integrations.

**Example Schema:**
```yaml
services:
  quanta_ethos_url: "http://localhost:8081/v1/governance/request_verdict"
  quanta_sensa_url: "http://localhost:8082/v1/metrics/usage_score"

pruning:
  # ... existing settings ...
  review_queue_path: "review_queue.log"
```

---

## 5. Testing Strategy

The testing strategy will evolve to handle live service dependencies gracefully.

-   **Unit Tests (`tests/unit/`):**
    -   Create tests for the new API clients (`ethos_client.py`, `sensa_client.py`), mocking the underlying HTTP library (e.g., `requests`) to ensure correct payload construction and response handling.
    -   Create tests for the `requirements.txt` parser to validate it can correctly identify dependencies.

-   **Integration Tests (`tests/integration/`):**
    -   The integration test suite will be enhanced to use **mock servers** (e.g., using `http.server` or a library like `pytest-httpserver`).
    -   These mock servers will be started during test setup and will respond to requests for `QuantaEthos` and `QuantaSensa`, allowing for end-to-end testing of the pruner's logic without requiring the actual services to be running.
    -   Add a test case to verify that a repository listed as a dependency is correctly identified and skipped.
    -   Add a test case to verify that a repository flagged for review is correctly written to the `review_queue.log`.

---

## 6. Future Enhancements (Post-Phase III)

Upon completion of Phase III, the project will be well-positioned for the following advanced capabilities:

-   **Advanced Dependency Analysis:** Expand the dependency parser to support more languages and package managers (e.g., `package.json` for Node.js, `pom.xml` for Java).
-   **Semantic Redundancy Analysis:** Integrate with an LLM via the `QuantaGlia-Harvester` to perform semantic analysis on repository content, allowing the pruner to identify functionally redundant repositories even if their code is not identical.
-   **Automated Merging:** Implement the logic to automatically merge repositories that are identified as highly redundant, consolidating knowledge and reducing clutter.