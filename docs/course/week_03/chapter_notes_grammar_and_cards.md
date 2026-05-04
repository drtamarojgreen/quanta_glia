# Chapter Notes: Green Syntax, Cards, and Sips

## 1. Green Syntax: The Language of the Greenhouse
Green Syntax is a structured, machine-readable format used to define SDD primitives. It is designed to be easily parsed by AI while remaining strictly bounded.

### Key Elements:
-   `SITUATION`: The environmental context.
-   `TOOLS`: The allowed instruments of construction.
-   `RESTRICTIONS`: The structural boundaries.
-   `EXPECTATIONS`: The desired factual outcome.

## 2. Cards: The Probes of Reality
A **Card** is a discrete unit of SDD logic (often implemented as a C++ class or Python script in this repo) that investigates a specific environmental condition.

-   **Anatomy of a Card:**
    -   `Setup()`: Prepares the probe.
    -   `Execute()`: Interacts with the environment.
    -   `Report()`: Appends a Fact to the Registry.

## 3. Sips: The Micro-Irrigation of Integration
A **Sip** (Small Integration Procedure) is a single execution of a Card.
-   Sips are atomic.
-   If a Sip fails, the development pipeline halts immediately.
-   This prevents "Environmental Contamination"—the spread of bugs across the greenhouse.

## 4. The SDD Workflow Pattern
1.  **Observe** the current Fact Registry.
2.  **Define** a new Restriction (Trellis).
3.  **Execute** a Sip to verify the Trellis.
4.  **Grow** the code within the verified Trellis.
