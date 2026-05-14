# SORREL Workflow Map: The Full SDD Loop

This map visually represents the full, iterative workflow of Sorrel Driven Development, showing how Facts, Cards, Runner, Observations, and Checkins interact in a continuous feedback loop driven by sips.

---

```mermaid
graph LR
    subgraph SDD Loop - Continuous Improvement
        A[Human: Define Next Sip Objective] --> B(Agent: Plan Card to Prove Objective)
        B --> C[Agent: Code Card (e.g., filesystem_create_file.cpp)]
        C --> D{Runner: Execute Card}
        D --> E[System: Observe Empirical Results]
        E --> F[System: Record Observations]
        F --> G[System: Update Facts]
        G --> H[System: Update sorrel_checkins.md]
        H --> I[Human: Review / Verify]
        I --> B
    end

    subgraph Key Artifacts
        FACTS["Facts (tests/sorrel/sdd/facts/environment.facts)"]
        CARDS["Card (tests/sorrel/sdd/cards/<name>.cpp)"]
        OBSERVATIONS["Empirical Observations (from Runner output)"]
        CHECKINS["sorrel_checkins.md"]
    end

    G --> FACTS
    C --> CARDS
    E --> OBSERVATIONS
    H --> CHECKINS

    %% Interaction details
    style A fill:#bde0e0,stroke:#333,stroke-width:2px;
    style B fill:#c2e0b2,stroke:#333,stroke-width:2px;
    style C fill:#c2e0b2,stroke:#333,stroke-width:2px;
    style D fill:#e0b2b2,stroke:#333,stroke-width:2px;
    style E fill:#e0e0b2,stroke:#333,stroke-width:2px;
    style F fill:#e0b2e0,stroke:#333,stroke-width:2px;
    style G fill:#b2e0c2,stroke:#333,stroke-width:2px;
    style H fill:#d0d0d0,stroke:#333,stroke-width:2px;
    style I fill:#bde0e0,stroke:#333,stroke-width:2px;

    linkStyle 0 stroke:#000,stroke-width:2px;
    linkStyle 1 stroke:#000,stroke-width:2px;
    linkStyle 2 stroke:#000,stroke-width:2px;
    linkStyle 3 stroke:#000,stroke-width:2px;
    linkStyle 4 stroke:#000,stroke-width:2px;
    linkStyle 5 stroke:#000,stroke-width:2px;
    linkStyle 6 stroke:#000,stroke-width:2px;
    linkStyle 7 stroke:#000,stroke-width:2px;

    linkStyle 8 stroke:#000,stroke-width:1px,fill:none;
    linkStyle 9 stroke:#000,stroke-width:1px,fill:none;
    linkStyle 10 stroke:#000,stroke-width:1px,fill:none;
    linkStyle 11 stroke:#000,stroke-width:1px,fill:none;
```

---

**Explanation of the Workflow:**

1.  **Human: Define Next Sip Objective:** The human developer initiates the cycle by identifying the single, minimal capability to be implemented and verified in the next "sip."
2.  **Agent: Plan Card to Prove Objective:** The AI agent plans the specific Card (an executable C++ unit) that will demonstrate or test this objective.
3.  **Agent: Code Card:** The AI agent writes the C++ source code for the Card (e.g., `filesystem_create_file.cpp`), placing it in `tests/sorrel/sdd/cards/`. This code must adhere to all SDD principles (minimal, no placeholders, empirical output only).
4.  **Runner: Execute Card:** The current SORREL Card Runner discovers a prebuilt executable for the card source and executes it with the card name. Compilation is a separate sip/check before the runner can collect evidence.
5.  **System: Observe Empirical Results:** The SORREL system observes and captures precise numeric outputs from the Card's execution (e.g., `filesystem_write_bytes = 6`, `created_file_count = 1`, `process_exit_code = 0`). These are the **Empirical Observations**. Boolean labels and pass/fail strings are not evidence.
6.  **System: Record Observations:** The observed empirical results are formatted and prepared for storage.
7.  **System: Update Facts:** The measured observations are appended as new numeric `Results` lines in the `tests/sorrel/sdd/facts/environment.facts` file. This updates the collective knowledge base of the environment's capabilities without converting evidence into pass/fail labels.
8.  **System: Update `sorrel_checkins.md`:** If the Card addressed any items from `sorrel_checkins.md`, or revealed new unimplemented work, this file is updated.
9.  **Human: Review / Verify:** The human reviews the updated Facts, observations, and any changes in the `sorrel_checkins.md` to ensure everything is correct and aligns with the objective. This is a critical human verification point.
10. **Agent: Continue Loop:** The cycle repeats, with the agent now having an updated set of Facts to inform its planning for the next sip.

This iterative loop ensures that SORREL development is disciplined, empirically grounded, and always moving forward from measured capabilities.

## Checkins and Checkouts in the Workflow

In addition to facts/cards, SDD workflow state is tracked through two companion logs:
1. **`sorrel_checkins.md`** for planned/deferred work.
2. **`sorrel_checkouts.md`** for completed sips and their empirical outcomes.

A workflow step is not fully closed until completed work is reflected in **checkouts** and any remaining future work is reflected in **checkins**.
