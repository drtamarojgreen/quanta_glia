# Week 9: Card Anatomy Deep-Dive—Probe Geometry and Reporting

## 1. The Anatomy of a Card
A **Card** is a specialized structural probe. It is the "eye" of the Greenhouse.

### The Three Sacred Methods:
1.  **`Setup()` (The Germination):** Prepares the environment and checks for the existence of the `SITUATION`.
2.  **`Execute()` (The Growth):** Performs the structural analysis or environment probe.
3.  **`Report()` (The Flowering):** Formats the findings into an immutable Fact and appends it to the Registry.

## 2. Probe Geometry
In your 3D mind-map, a Card is not just a file; it has **Geometry**.
-   **Shape:** A needle-like structure indicates a sharp, focused probe.
-   **Color:** The color changes based on the `Report()` status (Blue = Measuring, Green = Fact Verified, Red = Error).

## 3. Reporting Fidelity
A Card should never just return `true` or `false`. It should return **High-Fidelity Evidence**.
-   *Bad Report:* `is_clean = true`
-   *Good Report:* `line_count = 24, docstring_presence = 1.0, complexity_score = 4`

## 4. Lab: Dissecting a Card
Open `tests/sdd/cards/PrunerClass.cpp`.
1.  Identify the `Execute()` equivalent (the logic that calculates the composite score).
2.  Analyze how it uses the "Soil" (the `pruner.facts`).
3.  Rewrite the `Report()` logic to provide even higher fidelity data (e.g., specific weights used).
