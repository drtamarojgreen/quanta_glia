# Week 5: Complexity Restrictions—Folds, Branching, and Organic Density

## 1. Visualizing Complexity
In the Greenhouse, we do not view "Complexity" as a number, but as **Organic Density**. A module with high cyclomatic complexity looks like a tangled, overgrown thicket that prevents light (intent) from reaching the floor.

### The CDD "Pruning" Rule:
If the branching factor exceeds the "L-system" threshold of 3 nodes per branch, the module must be pruned.

## 2. Technical Restrictions: `LIMIT(complexity, value)`
We use Green Syntax to enforce structural simplicity.
-   **Nesting Depth:** Max 3 levels.
-   **Function Length:** Max 30 lines.
-   **Cyclomatic Complexity:** Max 10 per unit.

## 3. 3D Conceptual Translation: The "Biological Fold"
Imagine a piece of paper being folded. Each fold represents a logical branch (`if/else`, `switch`). Too many folds create a dense, heavy structure that is hard to unfold (debug).
-   **Goal:** Keep your "folds" rhythmic and purposeful. Avoid the "Maladaptive Thicket."

## 4. Lab: Measuring Density
Analyze a script in the `agent_utils/` directory.
1.  Count the number of logical branches.
2.  Translate this into a "Folding Pattern."
3.  Write a Green Syntax `LIMIT` operator that would have prevented any unnecessary branching you found.
