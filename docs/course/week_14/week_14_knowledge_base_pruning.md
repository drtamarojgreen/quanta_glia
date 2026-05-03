# Week 14: Knowledge Base Pruning—Decay, Rebirth, and Ethics

## 1. The Necessity of Decay
In a healthy ecosystem, old, redundant, or misleading information must decay to make room for new growth. We call this **Knowledge Base Pruning**.

## 2. The Pruner Logic
We use specialized algorithms (like those in `scripts/pruner.py`) to identify "Wilted Knowledge."
-   **Usage Score:** How often is this information accessed?
-   **Redundancy Score:** Is this information duplicated elsewhere?
-   **Age Score:** How old is this information relative to the current `SITUATION`?

## 3. The Ethics of Deletion
Deletion is a heavy act. We never delete "Ground Truth"—only "Derived Artifacts."
-   **Rule:** Keep the Roots (the original vision). Prune the Leaves (the temporary artifacts).

## 4. 3D Vision: Decay and Rebirth
Visualize the "Wilted Knowledge" as brown, curling leaves. When the Pruner runs, these leaves fall and dissolve into the soil (the Fact Registry), providing "nutrients" for the next cycle of growth.

## 5. Lab: Pruning Simulation
Examine `repository_data.csv`.
1.  Identify a repository with a low "usage_weight."
2.  Calculate its "Composite Score" using the formula in `PrunerClass.cpp`.
3.  Decide whether to `KEEP`, `ARCHIVE`, or `DELETE`.
