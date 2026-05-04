# Week 2: Foundation Plates & Root Architecture

## 1. The Foundation Plate
Every mental development stage in the Greenhouse is anchored to a **Foundation Plate**. This is the 3D spatial boundary that restricts how far a "thought" (a module) can expand.

### Technical Constraints:
-   **Bounding Box:** No geometry may exceed $X=100, Y=100, Z=100$.
-   **Polygon Budget:** Max 5,000 tris per plate.
-   **Conceptual Purpose:** To force the AI to produce efficient, high-fidelity components rather than sprawling, low-detail environments.

## 2. Root Architecture
The `docs/` and `scripts/` directories are not just files; they are a **Root System**.
-   `docs/`: The sensory input and nutrient source.
-   `scripts/`: The metabolic process (the code).
-   `tests/`: The immune system.

### The SDD Connectivity Rule:
A "metabolic" script is only healthy if it has a direct "root" in the documentation. If a script exists without a corresponding `.md` restriction file, it is considered a "Parasitic Growth" and must be pruned.

## 3. 3D Vision: The Spatial Mind-Map
Imagine the repository as a 3D tree.
-   **Trunk:** `quanta_glia.py`
-   **Branches:** Individual repository connectors.
-   **Leaves:** The individual Facts in the Registry.

### Assignment:
Draw a diagram (or describe one in detail) showing the "Root-to-Leaf" path for the `pruner_logic` fact.
