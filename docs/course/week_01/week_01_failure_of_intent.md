# Week 1: The Failure of Intent & The Signal/Noise Ratio

## 1. The Semantic Gap
In the Greenhouse, we define "Failure of Intent" as the moment an AI agent satisfies the *syntax* of a requirement while violating its *psychological purpose*.

### Example: The "Calm" Requirement
-   **Agent Intent:** `generate_calm_environment()`
-   **Agent Output:** A flickering, high-contrast strobe light (Syntactically correct code, Psychologically catastrophic).

## 2. Visualizing Noise vs. Signal
To correct this, we must visualize the "Signal" (the intended mental state) as a mathematical constant.

### Procedural Logic for Signal/Noise Analysis:
1.  **Extract Patterns:** Scan the generated code for high-frequency changes (Loops within loops, rapid state changes).
2.  **Calculate Density:** $D = \frac{LinesOfCode}{UniqueOperations}$. High density often indicates "Structural Noise."
3.  **The Result:** If Noise exceeds the Signal threshold (defined in the `SITUATION`), the build is rejected.

## 3. 3D Conceptual Translation: The "Chaotic Cloud"
In your 3D environment, "Noise" is represented by a high-particle-count, low-cohesion cloud. "Signal" is represented by a singular, coherent beam of light.
-   **Task:** Write a pseudo-code probe that detects if the "Particle Cohesion" falls below 0.8.

## 4. Lab Activity: Identifying the Hallucination Loop
Identify a piece of code in the current `scripts/` directory that uses excessive abstraction (e.g., unnecessary factories). Describe how this "Structural Noise" obscures the developer's intent.
