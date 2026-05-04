# Week 4: Fact Registry Probes & Protocols

## 1. The Fact Registry (The Empirical Soil)
The Fact Registry is the single source of truth for the entire Greenhouse. It is a persistent JSON/YAML file that tracks the state of every mental and technical component.

### Types of Facts:
1.  **Immutable Facts:** Set during initialization (e.g., `max_memory`).
2.  **Volatile Facts:** Changes with every build (e.g., `current_complexity`).
3.  **Derived Facts:** Calculated from other facts (e.g., `is_system_healthy`).

## 2. Writing a Probe (Card)
A **Probe** (or Card) is a script that executes inside the Greenhouse to report a Fact.

### Protocol for Probe Construction:
1.  **Safety First:** A probe must never modify the environment it is measuring.
2.  **Atomicity:** One probe = One fact.
3.  **High Fidelity:** Probes must return high-resolution data (e.g., `0.98` instead of `true`).

## 3. The Audit Trail
The Registry maintains a history of every Fact change. This is called the **Audit Trail**.
-   If the "Greenhouse" wilts (a regression occurs), we trace the trail back to the exact moment a Fact deviated from its expected value.

## 4. Assignment: Registry Mapping
Examine `tests/sdd/facts/pruner.facts`. List the five most critical facts and explain how they influence the `scripts/pruner.py` logic.
-   *Hint:* Look at `delete_threshold` and `usage_weight`.
