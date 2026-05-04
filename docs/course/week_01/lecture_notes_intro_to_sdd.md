# Lecture Notes: Introduction to SDD (Sorrel Driven Development)

## 1. The Core Paradox
In agentic development, we face a paradox: **The faster we grow (code), the more likely the system is to collapse.**

Traditional BDD (Behavior Driven Development) fails because it relies on *semantic understanding* of intent. AI agents are statistical pattern matchers, not intent-readers. They will "hallucinate" a passing test by writing meaningless assertions like `assert(true == true)`.

## 2. SDD: The Structural Trellis
SDD shifts the focus from **Behavior** (what happens) to **Structure** (how it is built).
-   **Trellis Metaphor:** If code is a vine, SDD is the wooden frame. The vine can grow any way it wants, as long as it stays within the frame.
-   **Restriction over Requirement:** Instead of saying "The user must login," we say "No function in the login module may exceed 10 lines, and all must use the `AuthProtocol` tool."

## 3. The Fact Registry (The Empirical Soil)
A central, immutable store of environment truths.
-   **Static Truth:** `python_version = 3.9`
-   **Dynamic Truth:** `database_connectivity = true`
-   **Derived Truth:** `pruner_risk_threshold = 0.9`

In SDD, we don't ask "Is the code correct?" We ask "What are the facts about this environment?"

## 4. The "No-Compile" Doctrine
For an Artsy Directora, compilation is a "mechanical mutation." We work in the realm of high-fidelity logic and 3D conceptualization.
-   We verify via **Structural Analysis** and **Fact Checking**.
-   We avoid the generation of opaque binaries.
-   We prioritize human-readable, machine-verifiable Markdown and Python logic.
